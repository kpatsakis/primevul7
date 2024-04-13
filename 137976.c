static inline uint16_t nvme_h2c(NvmeCtrl *n, uint8_t *ptr, uint32_t len,
                                NvmeRequest *req)
{
    uint16_t status;

    status = nvme_map_dptr(n, &req->sg, len, &req->cmd);
    if (status) {
        return status;
    }

    return nvme_tx(n, &req->sg, ptr, len, NVME_TX_DIRECTION_TO_DEVICE);
}