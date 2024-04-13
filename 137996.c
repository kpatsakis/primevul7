static uint16_t nvme_read(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    NvmeNamespace *ns = req->ns;
    uint64_t slba = le64_to_cpu(rw->slba);
    uint32_t nlb = (uint32_t)le16_to_cpu(rw->nlb) + 1;
    uint8_t prinfo = NVME_RW_PRINFO(le16_to_cpu(rw->control));
    uint64_t data_size = nvme_l2b(ns, nlb);
    uint64_t mapped_size = data_size;
    uint64_t data_offset;
    BlockBackend *blk = ns->blkconf.blk;
    uint16_t status;

    if (nvme_ns_ext(ns)) {
        mapped_size += nvme_m2b(ns, nlb);

        if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps)) {
            bool pract = prinfo & NVME_PRINFO_PRACT;

            if (pract && ns->lbaf.ms == 8) {
                mapped_size = data_size;
            }
        }
    }

    trace_pci_nvme_read(nvme_cid(req), nvme_nsid(ns), nlb, mapped_size, slba);

    status = nvme_check_mdts(n, mapped_size);
    if (status) {
        goto invalid;
    }

    status = nvme_check_bounds(ns, slba, nlb);
    if (status) {
        goto invalid;
    }

    if (ns->params.zoned) {
        status = nvme_check_zone_read(ns, slba, nlb);
        if (status) {
            trace_pci_nvme_err_zone_read_not_ok(slba, nlb, status);
            goto invalid;
        }
    }

    if (NVME_ERR_REC_DULBE(ns->features.err_rec)) {
        status = nvme_check_dulbe(ns, slba, nlb);
        if (status) {
            goto invalid;
        }
    }

    if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps)) {
        return nvme_dif_rw(n, req);
    }

    status = nvme_map_data(n, nlb, req);
    if (status) {
        goto invalid;
    }

    data_offset = nvme_l2b(ns, slba);

    block_acct_start(blk_get_stats(blk), &req->acct, data_size,
                     BLOCK_ACCT_READ);
    nvme_blk_read(blk, data_offset, nvme_rw_cb, req);
    return NVME_NO_COMPLETE;

invalid:
    block_acct_invalid(blk_get_stats(blk), BLOCK_ACCT_READ);
    return status | NVME_DNR;
}