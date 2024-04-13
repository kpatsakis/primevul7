static int nvme_start_ctrl(NvmeCtrl *n)
{
    uint64_t cap = ldq_le_p(&n->bar.cap);
    uint32_t cc = ldl_le_p(&n->bar.cc);
    uint32_t aqa = ldl_le_p(&n->bar.aqa);
    uint64_t asq = ldq_le_p(&n->bar.asq);
    uint64_t acq = ldq_le_p(&n->bar.acq);
    uint32_t page_bits = NVME_CC_MPS(cc) + 12;
    uint32_t page_size = 1 << page_bits;

    if (unlikely(n->cq[0])) {
        trace_pci_nvme_err_startfail_cq();
        return -1;
    }
    if (unlikely(n->sq[0])) {
        trace_pci_nvme_err_startfail_sq();
        return -1;
    }
    if (unlikely(asq & (page_size - 1))) {
        trace_pci_nvme_err_startfail_asq_misaligned(asq);
        return -1;
    }
    if (unlikely(acq & (page_size - 1))) {
        trace_pci_nvme_err_startfail_acq_misaligned(acq);
        return -1;
    }
    if (unlikely(!(NVME_CAP_CSS(cap) & (1 << NVME_CC_CSS(cc))))) {
        trace_pci_nvme_err_startfail_css(NVME_CC_CSS(cc));
        return -1;
    }
    if (unlikely(NVME_CC_MPS(cc) < NVME_CAP_MPSMIN(cap))) {
        trace_pci_nvme_err_startfail_page_too_small(
                    NVME_CC_MPS(cc),
                    NVME_CAP_MPSMIN(cap));
        return -1;
    }
    if (unlikely(NVME_CC_MPS(cc) >
                 NVME_CAP_MPSMAX(cap))) {
        trace_pci_nvme_err_startfail_page_too_large(
                    NVME_CC_MPS(cc),
                    NVME_CAP_MPSMAX(cap));
        return -1;
    }
    if (unlikely(NVME_CC_IOCQES(cc) <
                 NVME_CTRL_CQES_MIN(n->id_ctrl.cqes))) {
        trace_pci_nvme_err_startfail_cqent_too_small(
                    NVME_CC_IOCQES(cc),
                    NVME_CTRL_CQES_MIN(cap));
        return -1;
    }
    if (unlikely(NVME_CC_IOCQES(cc) >
                 NVME_CTRL_CQES_MAX(n->id_ctrl.cqes))) {
        trace_pci_nvme_err_startfail_cqent_too_large(
                    NVME_CC_IOCQES(cc),
                    NVME_CTRL_CQES_MAX(cap));
        return -1;
    }
    if (unlikely(NVME_CC_IOSQES(cc) <
                 NVME_CTRL_SQES_MIN(n->id_ctrl.sqes))) {
        trace_pci_nvme_err_startfail_sqent_too_small(
                    NVME_CC_IOSQES(cc),
                    NVME_CTRL_SQES_MIN(cap));
        return -1;
    }
    if (unlikely(NVME_CC_IOSQES(cc) >
                 NVME_CTRL_SQES_MAX(n->id_ctrl.sqes))) {
        trace_pci_nvme_err_startfail_sqent_too_large(
                    NVME_CC_IOSQES(cc),
                    NVME_CTRL_SQES_MAX(cap));
        return -1;
    }
    if (unlikely(!NVME_AQA_ASQS(aqa))) {
        trace_pci_nvme_err_startfail_asqent_sz_zero();
        return -1;
    }
    if (unlikely(!NVME_AQA_ACQS(aqa))) {
        trace_pci_nvme_err_startfail_acqent_sz_zero();
        return -1;
    }

    n->page_bits = page_bits;
    n->page_size = page_size;
    n->max_prp_ents = n->page_size / sizeof(uint64_t);
    n->cqe_size = 1 << NVME_CC_IOCQES(cc);
    n->sqe_size = 1 << NVME_CC_IOSQES(cc);
    nvme_init_cq(&n->admin_cq, n, acq, 0, 0, NVME_AQA_ACQS(aqa) + 1, 1);
    nvme_init_sq(&n->admin_sq, n, asq, 0, 0, NVME_AQA_ASQS(aqa) + 1);

    nvme_set_timestamp(n, 0ULL);

    QTAILQ_INIT(&n->aer_queue);

    nvme_select_iocs(n);

    return 0;
}