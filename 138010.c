static uint16_t nvme_set_feature(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeNamespace *ns = NULL;

    NvmeCmd *cmd = &req->cmd;
    uint32_t dw10 = le32_to_cpu(cmd->cdw10);
    uint32_t dw11 = le32_to_cpu(cmd->cdw11);
    uint32_t nsid = le32_to_cpu(cmd->nsid);
    uint8_t fid = NVME_GETSETFEAT_FID(dw10);
    uint8_t save = NVME_SETFEAT_SAVE(dw10);
    int i;

    trace_pci_nvme_setfeat(nvme_cid(req), nsid, fid, save, dw11);

    if (save && !(nvme_feature_cap[fid] & NVME_FEAT_CAP_SAVE)) {
        return NVME_FID_NOT_SAVEABLE | NVME_DNR;
    }

    if (!nvme_feature_support[fid]) {
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    if (nvme_feature_cap[fid] & NVME_FEAT_CAP_NS) {
        if (nsid != NVME_NSID_BROADCAST) {
            if (!nvme_nsid_valid(n, nsid)) {
                return NVME_INVALID_NSID | NVME_DNR;
            }

            ns = nvme_ns(n, nsid);
            if (unlikely(!ns)) {
                return NVME_INVALID_FIELD | NVME_DNR;
            }
        }
    } else if (nsid && nsid != NVME_NSID_BROADCAST) {
        if (!nvme_nsid_valid(n, nsid)) {
            return NVME_INVALID_NSID | NVME_DNR;
        }

        return NVME_FEAT_NOT_NS_SPEC | NVME_DNR;
    }

    if (!(nvme_feature_cap[fid] & NVME_FEAT_CAP_CHANGE)) {
        return NVME_FEAT_NOT_CHANGEABLE | NVME_DNR;
    }

    switch (fid) {
    case NVME_TEMPERATURE_THRESHOLD:
        if (NVME_TEMP_TMPSEL(dw11) != NVME_TEMP_TMPSEL_COMPOSITE) {
            break;
        }

        switch (NVME_TEMP_THSEL(dw11)) {
        case NVME_TEMP_THSEL_OVER:
            n->features.temp_thresh_hi = NVME_TEMP_TMPTH(dw11);
            break;
        case NVME_TEMP_THSEL_UNDER:
            n->features.temp_thresh_low = NVME_TEMP_TMPTH(dw11);
            break;
        default:
            return NVME_INVALID_FIELD | NVME_DNR;
        }

        if ((n->temperature >= n->features.temp_thresh_hi) ||
            (n->temperature <= n->features.temp_thresh_low)) {
            nvme_smart_event(n, NVME_AER_INFO_SMART_TEMP_THRESH);
        }

        break;
    case NVME_ERROR_RECOVERY:
        if (nsid == NVME_NSID_BROADCAST) {
            for (i = 1; i <= NVME_MAX_NAMESPACES; i++) {
                ns = nvme_ns(n, i);

                if (!ns) {
                    continue;
                }

                if (NVME_ID_NS_NSFEAT_DULBE(ns->id_ns.nsfeat)) {
                    ns->features.err_rec = dw11;
                }
            }

            break;
        }

        assert(ns);
        if (NVME_ID_NS_NSFEAT_DULBE(ns->id_ns.nsfeat))  {
            ns->features.err_rec = dw11;
        }
        break;
    case NVME_VOLATILE_WRITE_CACHE:
        for (i = 1; i <= NVME_MAX_NAMESPACES; i++) {
            ns = nvme_ns(n, i);
            if (!ns) {
                continue;
            }

            if (!(dw11 & 0x1) && blk_enable_write_cache(ns->blkconf.blk)) {
                blk_flush(ns->blkconf.blk);
            }

            blk_set_enable_write_cache(ns->blkconf.blk, dw11 & 1);
        }

        break;

    case NVME_NUMBER_OF_QUEUES:
        if (n->qs_created) {
            return NVME_CMD_SEQ_ERROR | NVME_DNR;
        }

        /*
         * NVMe v1.3, Section 5.21.1.7: FFFFh is not an allowed value for NCQR
         * and NSQR.
         */
        if ((dw11 & 0xffff) == 0xffff || ((dw11 >> 16) & 0xffff) == 0xffff) {
            return NVME_INVALID_FIELD | NVME_DNR;
        }

        trace_pci_nvme_setfeat_numq((dw11 & 0xffff) + 1,
                                    ((dw11 >> 16) & 0xffff) + 1,
                                    n->params.max_ioqpairs,
                                    n->params.max_ioqpairs);
        req->cqe.result = cpu_to_le32((n->params.max_ioqpairs - 1) |
                                      ((n->params.max_ioqpairs - 1) << 16));
        break;
    case NVME_ASYNCHRONOUS_EVENT_CONF:
        n->features.async_config = dw11;
        break;
    case NVME_TIMESTAMP:
        return nvme_set_feature_timestamp(n, req);
    case NVME_COMMAND_SET_PROFILE:
        if (dw11 & 0x1ff) {
            trace_pci_nvme_err_invalid_iocsci(dw11 & 0x1ff);
            return NVME_CMD_SET_CMB_REJECTED | NVME_DNR;
        }
        break;
    default:
        return NVME_FEAT_NOT_CHANGEABLE | NVME_DNR;
    }
    return NVME_SUCCESS;
}