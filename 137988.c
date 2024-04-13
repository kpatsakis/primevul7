static void nvme_select_iocs_ns(NvmeCtrl *n, NvmeNamespace *ns)
{
    uint32_t cc = ldl_le_p(&n->bar.cc);

    ns->iocs = nvme_cse_iocs_none;
    switch (ns->csi) {
    case NVME_CSI_NVM:
        if (NVME_CC_CSS(cc) != NVME_CC_CSS_ADMIN_ONLY) {
            ns->iocs = nvme_cse_iocs_nvm;
        }
        break;
    case NVME_CSI_ZONED:
        if (NVME_CC_CSS(cc) == NVME_CC_CSS_CSI) {
            ns->iocs = nvme_cse_iocs_zoned;
        } else if (NVME_CC_CSS(cc) == NVME_CC_CSS_NVM) {
            ns->iocs = nvme_cse_iocs_nvm;
        }
        break;
    }
}