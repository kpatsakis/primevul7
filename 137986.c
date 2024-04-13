static void nvme_copy_out_completed_cb(void *opaque, int ret)
{
    NvmeCopyAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;
    NvmeCopySourceRange *range = &iocb->ranges[iocb->idx];
    uint32_t nlb = le32_to_cpu(range->nlb) + 1;

    if (ret < 0) {
        iocb->ret = ret;
        goto out;
    } else if (iocb->ret < 0) {
        goto out;
    }

    if (ns->params.zoned) {
        nvme_advance_zone_wp(ns, iocb->zone, nlb);
    }

    iocb->idx++;
    iocb->slba += nlb;
out:
    nvme_copy_cb(iocb, iocb->ret);
}