static uint16_t nvme_dsm(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeNamespace *ns = req->ns;
    NvmeDsmCmd *dsm = (NvmeDsmCmd *) &req->cmd;
    uint32_t attr = le32_to_cpu(dsm->attributes);
    uint32_t nr = (le32_to_cpu(dsm->nr) & 0xff) + 1;
    uint16_t status = NVME_SUCCESS;

    trace_pci_nvme_dsm(nr, attr);

    if (attr & NVME_DSMGMT_AD) {
        NvmeDSMAIOCB *iocb = blk_aio_get(&nvme_dsm_aiocb_info, ns->blkconf.blk,
                                         nvme_misc_cb, req);

        iocb->req = req;
        iocb->bh = qemu_bh_new(nvme_dsm_bh, iocb);
        iocb->ret = 0;
        iocb->range = g_new(NvmeDsmRange, nr);
        iocb->nr = nr;
        iocb->idx = 0;

        status = nvme_h2c(n, (uint8_t *)iocb->range, sizeof(NvmeDsmRange) * nr,
                          req);
        if (status) {
            return status;
        }

        req->aiocb = &iocb->common;
        nvme_dsm_cb(iocb, 0);

        return NVME_NO_COMPLETE;
    }

    return status;
}