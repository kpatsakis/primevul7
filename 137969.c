static uint64_t nvme_mmio_read(void *opaque, hwaddr addr, unsigned size)
{
    NvmeCtrl *n = (NvmeCtrl *)opaque;
    uint8_t *ptr = (uint8_t *)&n->bar;

    trace_pci_nvme_mmio_read(addr, size);

    if (unlikely(addr & (sizeof(uint32_t) - 1))) {
        NVME_GUEST_ERR(pci_nvme_ub_mmiord_misaligned32,
                       "MMIO read not 32-bit aligned,"
                       " offset=0x%"PRIx64"", addr);
        /* should RAZ, fall through for now */
    } else if (unlikely(size < sizeof(uint32_t))) {
        NVME_GUEST_ERR(pci_nvme_ub_mmiord_toosmall,
                       "MMIO read smaller than 32-bits,"
                       " offset=0x%"PRIx64"", addr);
        /* should RAZ, fall through for now */
    }

    if (addr > sizeof(n->bar) - size) {
        NVME_GUEST_ERR(pci_nvme_ub_mmiord_invalid_ofs,
                       "MMIO read beyond last register,"
                       " offset=0x%"PRIx64", returning 0", addr);

        return 0;
    }

    /*
     * When PMRWBM bit 1 is set then read from
     * from PMRSTS should ensure prior writes
     * made it to persistent media
     */
    if (addr == NVME_REG_PMRSTS &&
        (NVME_PMRCAP_PMRWBM(ldl_le_p(&n->bar.pmrcap)) & 0x02)) {
        memory_region_msync(&n->pmr.dev->mr, 0, n->pmr.dev->size);
    }

    return ldn_le_p(ptr + addr, size);
}