static uint16_t nvme_do_write(NvmeCtrl *n, NvmeRequest *req, bool append,
                              bool wrz)
{
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    NvmeNamespace *ns = req->ns;
    uint64_t slba = le64_to_cpu(rw->slba);
    uint32_t nlb = (uint32_t)le16_to_cpu(rw->nlb) + 1;
    uint16_t ctrl = le16_to_cpu(rw->control);
    uint8_t prinfo = NVME_RW_PRINFO(ctrl);
    uint64_t data_size = nvme_l2b(ns, nlb);
    uint64_t mapped_size = data_size;
    uint64_t data_offset;
    NvmeZone *zone;
    NvmeZonedResult *res = (NvmeZonedResult *)&req->cqe;
    BlockBackend *blk = ns->blkconf.blk;
    uint16_t status;

    if (nvme_ns_ext(ns)) {
        mapped_size += nvme_m2b(ns, nlb);

        if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps)) {
            bool pract = prinfo & NVME_PRINFO_PRACT;

            if (pract && ns->lbaf.ms == 8) {
                mapped_size -= nvme_m2b(ns, nlb);
            }
        }
    }

    trace_pci_nvme_write(nvme_cid(req), nvme_io_opc_str(rw->opcode),
                         nvme_nsid(ns), nlb, mapped_size, slba);

    if (!wrz) {
        status = nvme_check_mdts(n, mapped_size);
        if (status) {
            goto invalid;
        }
    }

    status = nvme_check_bounds(ns, slba, nlb);
    if (status) {
        goto invalid;
    }

    if (ns->params.zoned) {
        zone = nvme_get_zone_by_slba(ns, slba);
        assert(zone);

        if (append) {
            bool piremap = !!(ctrl & NVME_RW_PIREMAP);

            if (unlikely(slba != zone->d.zslba)) {
                trace_pci_nvme_err_append_not_at_start(slba, zone->d.zslba);
                status = NVME_INVALID_FIELD;
                goto invalid;
            }

            if (n->params.zasl &&
                data_size > (uint64_t)n->page_size << n->params.zasl) {
                trace_pci_nvme_err_zasl(data_size);
                return NVME_INVALID_FIELD | NVME_DNR;
            }

            slba = zone->w_ptr;
            rw->slba = cpu_to_le64(slba);
            res->slba = cpu_to_le64(slba);

            switch (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps)) {
            case NVME_ID_NS_DPS_TYPE_1:
                if (!piremap) {
                    return NVME_INVALID_PROT_INFO | NVME_DNR;
                }

                /* fallthrough */

            case NVME_ID_NS_DPS_TYPE_2:
                if (piremap) {
                    uint32_t reftag = le32_to_cpu(rw->reftag);
                    rw->reftag = cpu_to_le32(reftag + (slba - zone->d.zslba));
                }

                break;

            case NVME_ID_NS_DPS_TYPE_3:
                if (piremap) {
                    return NVME_INVALID_PROT_INFO | NVME_DNR;
                }

                break;
            }
        }

        status = nvme_check_zone_write(ns, zone, slba, nlb);
        if (status) {
            goto invalid;
        }

        status = nvme_zrm_auto(n, ns, zone);
        if (status) {
            goto invalid;
        }

        zone->w_ptr += nlb;
    }

    data_offset = nvme_l2b(ns, slba);

    if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps)) {
        return nvme_dif_rw(n, req);
    }

    if (!wrz) {
        status = nvme_map_data(n, nlb, req);
        if (status) {
            goto invalid;
        }

        block_acct_start(blk_get_stats(blk), &req->acct, data_size,
                         BLOCK_ACCT_WRITE);
        nvme_blk_write(blk, data_offset, nvme_rw_cb, req);
    } else {
        req->aiocb = blk_aio_pwrite_zeroes(blk, data_offset, data_size,
                                           BDRV_REQ_MAY_UNMAP, nvme_rw_cb,
                                           req);
    }

    return NVME_NO_COMPLETE;

invalid:
    block_acct_invalid(blk_get_stats(blk), BLOCK_ACCT_WRITE);
    return status | NVME_DNR;
}