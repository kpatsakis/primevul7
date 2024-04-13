static void nvme_misc_cb(void *opaque, int ret)
{
    NvmeRequest *req = opaque;

    trace_pci_nvme_misc_cb(nvme_cid(req));

    if (ret) {
        nvme_aio_err(req, ret);
    }

    nvme_enqueue_req_completion(nvme_cq(req), req);
}