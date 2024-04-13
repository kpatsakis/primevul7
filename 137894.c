static inline uint16_t nvme_write(NvmeCtrl *n, NvmeRequest *req)
{
    return nvme_do_write(n, req, false, false);
}