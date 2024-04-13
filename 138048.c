static void nvme_sg_split(NvmeSg *sg, NvmeNamespace *ns, NvmeSg *data,
                          NvmeSg *mdata)
{
    NvmeSg *dst = data;
    uint32_t trans_len, count = ns->lbasz;
    uint64_t offset = 0;
    bool dma = sg->flags & NVME_SG_DMA;
    size_t sge_len;
    size_t sg_len = dma ? sg->qsg.size : sg->iov.size;
    int sg_idx = 0;

    assert(sg->flags & NVME_SG_ALLOC);

    while (sg_len) {
        sge_len = dma ? sg->qsg.sg[sg_idx].len : sg->iov.iov[sg_idx].iov_len;

        trans_len = MIN(sg_len, count);
        trans_len = MIN(trans_len, sge_len - offset);

        if (dst) {
            if (dma) {
                qemu_sglist_add(&dst->qsg, sg->qsg.sg[sg_idx].base + offset,
                                trans_len);
            } else {
                qemu_iovec_add(&dst->iov,
                               sg->iov.iov[sg_idx].iov_base + offset,
                               trans_len);
            }
        }

        sg_len -= trans_len;
        count -= trans_len;
        offset += trans_len;

        if (count == 0) {
            dst = (dst == data) ? mdata : data;
            count = (dst == data) ? ns->lbasz : ns->lbaf.ms;
        }

        if (sge_len == offset) {
            offset = 0;
            sg_idx++;
        }
    }
}