static uint16_t nvme_copy(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeNamespace *ns = req->ns;
    NvmeCopyCmd *copy = (NvmeCopyCmd *)&req->cmd;
    NvmeCopyAIOCB *iocb = blk_aio_get(&nvme_copy_aiocb_info, ns->blkconf.blk,
                                      nvme_misc_cb, req);
    uint16_t nr = copy->nr + 1;
    uint8_t format = copy->control[0] & 0xf;
    uint16_t prinfor = ((copy->control[0] >> 4) & 0xf);
    uint16_t prinfow = ((copy->control[2] >> 2) & 0xf);

    uint16_t status;

    trace_pci_nvme_copy(nvme_cid(req), nvme_nsid(ns), nr, format);

    iocb->ranges = NULL;
    iocb->zone = NULL;

    if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps) &&
        ((prinfor & NVME_PRINFO_PRACT) != (prinfow & NVME_PRINFO_PRACT))) {
        status = NVME_INVALID_FIELD | NVME_DNR;
        goto invalid;
    }

    if (!(n->id_ctrl.ocfs & (1 << format))) {
        trace_pci_nvme_err_copy_invalid_format(format);
        status = NVME_INVALID_FIELD | NVME_DNR;
        goto invalid;
    }

    if (nr > ns->id_ns.msrc + 1) {
        status = NVME_CMD_SIZE_LIMIT | NVME_DNR;
        goto invalid;
    }

    iocb->ranges = g_new(NvmeCopySourceRange, nr);

    status = nvme_h2c(n, (uint8_t *)iocb->ranges,
                      sizeof(NvmeCopySourceRange) * nr, req);
    if (status) {
        goto invalid;
    }

    iocb->slba = le64_to_cpu(copy->sdlba);

    if (ns->params.zoned) {
        iocb->zone = nvme_get_zone_by_slba(ns, iocb->slba);
        if (!iocb->zone) {
            status = NVME_LBA_RANGE | NVME_DNR;
            goto invalid;
        }

        status = nvme_zrm_auto(n, ns, iocb->zone);
        if (status) {
            goto invalid;
        }
    }

    iocb->req = req;
    iocb->bh = qemu_bh_new(nvme_copy_bh, iocb);
    iocb->ret = 0;
    iocb->nr = nr;
    iocb->idx = 0;
    iocb->reftag = le32_to_cpu(copy->reftag);
    iocb->bounce = g_malloc_n(le16_to_cpu(ns->id_ns.mssrl),
                              ns->lbasz + ns->lbaf.ms);

    qemu_iovec_init(&iocb->iov, 1);

    block_acct_start(blk_get_stats(ns->blkconf.blk), &iocb->acct.read, 0,
                     BLOCK_ACCT_READ);
    block_acct_start(blk_get_stats(ns->blkconf.blk), &iocb->acct.write, 0,
                     BLOCK_ACCT_WRITE);

    req->aiocb = &iocb->common;
    nvme_copy_cb(iocb, 0);

    return NVME_NO_COMPLETE;

invalid:
    g_free(iocb->ranges);
    qemu_aio_unref(iocb);
    return status;
}