static inline uint16_t nvme_check_bounds(NvmeNamespace *ns, uint64_t slba,
                                         uint32_t nlb)
{
    uint64_t nsze = le64_to_cpu(ns->id_ns.nsze);

    if (unlikely(UINT64_MAX - slba < nlb || slba + nlb > nsze)) {
        trace_pci_nvme_err_invalid_lba_range(slba, nlb, nsze);
        return NVME_LBA_RANGE | NVME_DNR;
    }

    return NVME_SUCCESS;
}