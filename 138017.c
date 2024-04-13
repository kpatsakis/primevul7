static uint16_t nvme_get_feature_timestamp(NvmeCtrl *n, NvmeRequest *req)
{
    uint64_t timestamp = nvme_get_timestamp(n);

    return nvme_c2h(n, (uint8_t *)&timestamp, sizeof(timestamp), req);
}