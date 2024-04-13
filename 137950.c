static uint16_t nvme_tx_interleaved(NvmeCtrl *n, NvmeSg *sg, uint8_t *ptr,
                                    uint32_t len, uint32_t bytes,
                                    int32_t skip_bytes, int64_t offset,
                                    NvmeTxDirection dir)
{
    hwaddr addr;
    uint32_t trans_len, count = bytes;
    bool dma = sg->flags & NVME_SG_DMA;
    int64_t sge_len;
    int sg_idx = 0;
    int ret;

    assert(sg->flags & NVME_SG_ALLOC);

    while (len) {
        sge_len = dma ? sg->qsg.sg[sg_idx].len : sg->iov.iov[sg_idx].iov_len;

        if (sge_len - offset < 0) {
            offset -= sge_len;
            sg_idx++;
            continue;
        }

        if (sge_len == offset) {
            offset = 0;
            sg_idx++;
            continue;
        }

        trans_len = MIN(len, count);
        trans_len = MIN(trans_len, sge_len - offset);

        if (dma) {
            addr = sg->qsg.sg[sg_idx].base + offset;
        } else {
            addr = (hwaddr)(uintptr_t)sg->iov.iov[sg_idx].iov_base + offset;
        }

        if (dir == NVME_TX_DIRECTION_TO_DEVICE) {
            ret = nvme_addr_read(n, addr, ptr, trans_len);
        } else {
            ret = nvme_addr_write(n, addr, ptr, trans_len);
        }

        if (ret) {
            return NVME_DATA_TRAS_ERROR;
        }

        ptr += trans_len;
        len -= trans_len;
        count -= trans_len;
        offset += trans_len;

        if (count == 0) {
            count = bytes;
            offset += skip_bytes;
        }
    }

    return NVME_SUCCESS;
}