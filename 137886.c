static uint16_t nvme_zone_mgmt_send(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeCmd *cmd = (NvmeCmd *)&req->cmd;
    NvmeNamespace *ns = req->ns;
    NvmeZone *zone;
    NvmeZoneResetAIOCB *iocb;
    uint8_t *zd_ext;
    uint32_t dw13 = le32_to_cpu(cmd->cdw13);
    uint64_t slba = 0;
    uint32_t zone_idx = 0;
    uint16_t status;
    uint8_t action;
    bool all;
    enum NvmeZoneProcessingMask proc_mask = NVME_PROC_CURRENT_ZONE;

    action = dw13 & 0xff;
    all = !!(dw13 & 0x100);

    req->status = NVME_SUCCESS;

    if (!all) {
        status = nvme_get_mgmt_zone_slba_idx(ns, cmd, &slba, &zone_idx);
        if (status) {
            return status;
        }
    }

    zone = &ns->zone_array[zone_idx];
    if (slba != zone->d.zslba) {
        trace_pci_nvme_err_unaligned_zone_cmd(action, slba, zone->d.zslba);
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    switch (action) {

    case NVME_ZONE_ACTION_OPEN:
        if (all) {
            proc_mask = NVME_PROC_CLOSED_ZONES;
        }
        trace_pci_nvme_open_zone(slba, zone_idx, all);
        status = nvme_do_zone_op(ns, zone, proc_mask, nvme_open_zone, req);
        break;

    case NVME_ZONE_ACTION_CLOSE:
        if (all) {
            proc_mask = NVME_PROC_OPENED_ZONES;
        }
        trace_pci_nvme_close_zone(slba, zone_idx, all);
        status = nvme_do_zone_op(ns, zone, proc_mask, nvme_close_zone, req);
        break;

    case NVME_ZONE_ACTION_FINISH:
        if (all) {
            proc_mask = NVME_PROC_OPENED_ZONES | NVME_PROC_CLOSED_ZONES;
        }
        trace_pci_nvme_finish_zone(slba, zone_idx, all);
        status = nvme_do_zone_op(ns, zone, proc_mask, nvme_finish_zone, req);
        break;

    case NVME_ZONE_ACTION_RESET:
        trace_pci_nvme_reset_zone(slba, zone_idx, all);

        iocb = blk_aio_get(&nvme_zone_reset_aiocb_info, ns->blkconf.blk,
                           nvme_misc_cb, req);

        iocb->req = req;
        iocb->bh = qemu_bh_new(nvme_zone_reset_bh, iocb);
        iocb->ret = 0;
        iocb->all = all;
        iocb->idx = zone_idx;
        iocb->zone = NULL;

        req->aiocb = &iocb->common;
        nvme_zone_reset_cb(iocb, 0);

        return NVME_NO_COMPLETE;

    case NVME_ZONE_ACTION_OFFLINE:
        if (all) {
            proc_mask = NVME_PROC_READ_ONLY_ZONES;
        }
        trace_pci_nvme_offline_zone(slba, zone_idx, all);
        status = nvme_do_zone_op(ns, zone, proc_mask, nvme_offline_zone, req);
        break;

    case NVME_ZONE_ACTION_SET_ZD_EXT:
        trace_pci_nvme_set_descriptor_extension(slba, zone_idx);
        if (all || !ns->params.zd_extension_size) {
            return NVME_INVALID_FIELD | NVME_DNR;
        }
        zd_ext = nvme_get_zd_extension(ns, zone_idx);
        status = nvme_h2c(n, zd_ext, ns->params.zd_extension_size, req);
        if (status) {
            trace_pci_nvme_err_zd_extension_map_error(zone_idx);
            return status;
        }

        status = nvme_set_zd_ext(ns, zone);
        if (status == NVME_SUCCESS) {
            trace_pci_nvme_zd_extension_set(zone_idx);
            return status;
        }
        break;

    default:
        trace_pci_nvme_err_invalid_mgmt_action(action);
        status = NVME_INVALID_FIELD;
    }

    if (status == NVME_ZONE_INVAL_TRANSITION) {
        trace_pci_nvme_err_invalid_zone_state_transition(action, slba,
                                                         zone->d.za);
    }
    if (status) {
        status |= NVME_DNR;
    }

    return status;
}