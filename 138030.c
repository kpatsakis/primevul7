static inline uint16_t nvme_zone_append(NvmeCtrl *n, NvmeRequest *req)
{
    return nvme_do_write(n, req, true, false);
}