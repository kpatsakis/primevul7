static void nvme_zone_reset_cb(void *opaque, int ret)
{
    NvmeZoneResetAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;

    if (ret < 0) {
        iocb->ret = ret;
        goto done;
    }

    if (iocb->zone) {
        nvme_zrm_reset(ns, iocb->zone);

        if (!iocb->all) {
            goto done;
        }
    }

    while (iocb->idx < ns->num_zones) {
        NvmeZone *zone = &ns->zone_array[iocb->idx++];

        switch (nvme_get_zone_state(zone)) {
        case NVME_ZONE_STATE_EMPTY:
            if (!iocb->all) {
                goto done;
            }

            continue;

        case NVME_ZONE_STATE_EXPLICITLY_OPEN:
        case NVME_ZONE_STATE_IMPLICITLY_OPEN:
        case NVME_ZONE_STATE_CLOSED:
        case NVME_ZONE_STATE_FULL:
            iocb->zone = zone;
            break;

        default:
            continue;
        }

        trace_pci_nvme_zns_zone_reset(zone->d.zslba);

        iocb->aiocb = blk_aio_pwrite_zeroes(ns->blkconf.blk,
                                            nvme_l2b(ns, zone->d.zslba),
                                            nvme_l2b(ns, ns->zone_size),
                                            BDRV_REQ_MAY_UNMAP,
                                            nvme_zone_reset_epilogue_cb,
                                            iocb);
        return;
    }

done:
    iocb->aiocb = NULL;
    if (iocb->bh) {
        qemu_bh_schedule(iocb->bh);
    }
}