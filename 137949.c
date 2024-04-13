static void nvme_copy_in_completed_cb(void *opaque, int ret)
{
    NvmeCopyAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;
    NvmeCopySourceRange *range;
    uint32_t nlb;
    size_t len;
    uint16_t status;

    if (ret < 0) {
        iocb->ret = ret;
        goto out;
    } else if (iocb->ret < 0) {
        goto out;
    }

    range = &iocb->ranges[iocb->idx];
    nlb = le32_to_cpu(range->nlb) + 1;
    len = nvme_l2b(ns, nlb);

    trace_pci_nvme_copy_out(iocb->slba, nlb);

    if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps)) {
        NvmeCopyCmd *copy = (NvmeCopyCmd *)&req->cmd;

        uint16_t prinfor = ((copy->control[0] >> 4) & 0xf);
        uint16_t prinfow = ((copy->control[2] >> 2) & 0xf);

        uint16_t apptag = le16_to_cpu(range->apptag);
        uint16_t appmask = le16_to_cpu(range->appmask);
        uint32_t reftag = le32_to_cpu(range->reftag);

        uint64_t slba = le64_to_cpu(range->slba);
        size_t mlen = nvme_m2b(ns, nlb);
        uint8_t *mbounce = iocb->bounce + nvme_l2b(ns, nlb);

        status = nvme_dif_check(ns, iocb->bounce, len, mbounce, mlen, prinfor,
                                slba, apptag, appmask, &reftag);
        if (status) {
            goto invalid;
        }

        apptag = le16_to_cpu(copy->apptag);
        appmask = le16_to_cpu(copy->appmask);

        if (prinfow & NVME_PRINFO_PRACT) {
            status = nvme_check_prinfo(ns, prinfow, iocb->slba, iocb->reftag);
            if (status) {
                goto invalid;
            }

            nvme_dif_pract_generate_dif(ns, iocb->bounce, len, mbounce, mlen,
                                        apptag, &iocb->reftag);
        } else {
            status = nvme_dif_check(ns, iocb->bounce, len, mbounce, mlen,
                                    prinfow, iocb->slba, apptag, appmask,
                                    &iocb->reftag);
            if (status) {
                goto invalid;
            }
        }
    }

    status = nvme_check_bounds(ns, iocb->slba, nlb);
    if (status) {
        goto invalid;
    }

    if (ns->params.zoned) {
        status = nvme_check_zone_write(ns, iocb->zone, iocb->slba, nlb);
        if (status) {
            goto invalid;
        }

        iocb->zone->w_ptr += nlb;
    }

    qemu_iovec_reset(&iocb->iov);
    qemu_iovec_add(&iocb->iov, iocb->bounce, len);

    iocb->aiocb = blk_aio_pwritev(ns->blkconf.blk, nvme_l2b(ns, iocb->slba),
                                  &iocb->iov, 0, nvme_copy_out_cb, iocb);

    return;

invalid:
    req->status = status;
    iocb->aiocb = NULL;
    if (iocb->bh) {
        qemu_bh_schedule(iocb->bh);
    }

    return;

out:
    nvme_copy_cb(iocb, ret);
}