static inline uint16_t nvme_write_zeroes(NvmeCtrl *n, NvmeRequest *req)
{
    return nvme_do_write(n, req, false, true);
}