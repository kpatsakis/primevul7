static uint16_t nvme_tx(NvmeCtrl *n, NvmeSg *sg, uint8_t *ptr, uint32_t len,
                        NvmeTxDirection dir)
{
    assert(sg->flags & NVME_SG_ALLOC);

    if (sg->flags & NVME_SG_DMA) {
        uint64_t residual;

        if (dir == NVME_TX_DIRECTION_TO_DEVICE) {
            residual = dma_buf_write(ptr, len, &sg->qsg);
        } else {
            residual = dma_buf_read(ptr, len, &sg->qsg);
        }

        if (unlikely(residual)) {
            trace_pci_nvme_err_invalid_dma();
            return NVME_INVALID_FIELD | NVME_DNR;
        }
    } else {
        size_t bytes;

        if (dir == NVME_TX_DIRECTION_TO_DEVICE) {
            bytes = qemu_iovec_to_buf(&sg->iov, 0, ptr, len);
        } else {
            bytes = qemu_iovec_from_buf(&sg->iov, 0, ptr, len);
        }

        if (unlikely(bytes != len)) {
            trace_pci_nvme_err_invalid_dma();
            return NVME_INVALID_FIELD | NVME_DNR;
        }
    }

    return NVME_SUCCESS;
}