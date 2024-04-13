static void nvme_dsm_cb(void *opaque, int ret)
{
    NvmeDSMAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeCtrl *n = nvme_ctrl(req);
    NvmeNamespace *ns = req->ns;
    NvmeDsmRange *range;
    uint64_t slba;
    uint32_t nlb;

    if (ret < 0) {
        iocb->ret = ret;
        goto done;
    }

next:
    if (iocb->idx == iocb->nr) {
        goto done;
    }

    range = &iocb->range[iocb->idx++];
    slba = le64_to_cpu(range->slba);
    nlb = le32_to_cpu(range->nlb);

    trace_pci_nvme_dsm_deallocate(slba, nlb);

    if (nlb > n->dmrsl) {
        trace_pci_nvme_dsm_single_range_limit_exceeded(nlb, n->dmrsl);
        goto next;
    }

    if (nvme_check_bounds(ns, slba, nlb)) {
        trace_pci_nvme_err_invalid_lba_range(slba, nlb,
                                             ns->id_ns.nsze);
        goto next;
    }

    iocb->aiocb = blk_aio_pdiscard(ns->blkconf.blk, nvme_l2b(ns, slba),
                                   nvme_l2b(ns, nlb),
                                   nvme_dsm_md_cb, iocb);
    return;

done:
    iocb->aiocb = NULL;
    qemu_bh_schedule(iocb->bh);
}