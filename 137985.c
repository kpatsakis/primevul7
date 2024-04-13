static uint16_t nvme_cmd_effects(NvmeCtrl *n, uint8_t csi, uint32_t buf_len,
                                 uint64_t off, NvmeRequest *req)
{
    NvmeEffectsLog log = {};
    const uint32_t *src_iocs = NULL;
    uint32_t trans_len;

    if (off >= sizeof(log)) {
        trace_pci_nvme_err_invalid_log_page_offset(off, sizeof(log));
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    switch (NVME_CC_CSS(ldl_le_p(&n->bar.cc))) {
    case NVME_CC_CSS_NVM:
        src_iocs = nvme_cse_iocs_nvm;
        /* fall through */
    case NVME_CC_CSS_ADMIN_ONLY:
        break;
    case NVME_CC_CSS_CSI:
        switch (csi) {
        case NVME_CSI_NVM:
            src_iocs = nvme_cse_iocs_nvm;
            break;
        case NVME_CSI_ZONED:
            src_iocs = nvme_cse_iocs_zoned;
            break;
        }
    }

    memcpy(log.acs, nvme_cse_acs, sizeof(nvme_cse_acs));

    if (src_iocs) {
        memcpy(log.iocs, src_iocs, sizeof(log.iocs));
    }

    trans_len = MIN(sizeof(log) - off, buf_len);

    return nvme_c2h(n, ((uint8_t *)&log) + off, trans_len, req);
}