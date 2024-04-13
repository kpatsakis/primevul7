static void nvme_rw_cb(void *opaque, int ret)
{
    NvmeRequest *req = opaque;
    NvmeNamespace *ns = req->ns;

    BlockBackend *blk = ns->blkconf.blk;

    trace_pci_nvme_rw_cb(nvme_cid(req), blk_name(blk));

    if (ret) {
        goto out;
    }

    if (ns->lbaf.ms) {
        NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
        uint64_t slba = le64_to_cpu(rw->slba);
        uint32_t nlb = (uint32_t)le16_to_cpu(rw->nlb) + 1;
        uint64_t offset = nvme_moff(ns, slba);

        if (req->cmd.opcode == NVME_CMD_WRITE_ZEROES) {
            size_t mlen = nvme_m2b(ns, nlb);

            req->aiocb = blk_aio_pwrite_zeroes(blk, offset, mlen,
                                               BDRV_REQ_MAY_UNMAP,
                                               nvme_rw_complete_cb, req);
            return;
        }

        if (nvme_ns_ext(ns) || req->cmd.mptr) {
            uint16_t status;

            nvme_sg_unmap(&req->sg);
            status = nvme_map_mdata(nvme_ctrl(req), nlb, req);
            if (status) {
                ret = -EFAULT;
                goto out;
            }

            if (req->cmd.opcode == NVME_CMD_READ) {
                return nvme_blk_read(blk, offset, nvme_rw_complete_cb, req);
            }

            return nvme_blk_write(blk, offset, nvme_rw_complete_cb, req);
        }
    }

out:
    nvme_rw_complete_cb(req, ret);
}