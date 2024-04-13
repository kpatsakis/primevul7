static void nvme_init_ctrl(NvmeCtrl *n, PCIDevice *pci_dev)
{
    NvmeIdCtrl *id = &n->id_ctrl;
    uint8_t *pci_conf = pci_dev->config;
    uint64_t cap = ldq_le_p(&n->bar.cap);

    id->vid = cpu_to_le16(pci_get_word(pci_conf + PCI_VENDOR_ID));
    id->ssvid = cpu_to_le16(pci_get_word(pci_conf + PCI_SUBSYSTEM_VENDOR_ID));
    strpadcpy((char *)id->mn, sizeof(id->mn), "QEMU NVMe Ctrl", ' ');
    strpadcpy((char *)id->fr, sizeof(id->fr), "1.0", ' ');
    strpadcpy((char *)id->sn, sizeof(id->sn), n->params.serial, ' ');

    id->cntlid = cpu_to_le16(n->cntlid);

    id->oaes = cpu_to_le32(NVME_OAES_NS_ATTR);

    id->rab = 6;

    if (n->params.use_intel_id) {
        id->ieee[0] = 0xb3;
        id->ieee[1] = 0x02;
        id->ieee[2] = 0x00;
    } else {
        id->ieee[0] = 0x00;
        id->ieee[1] = 0x54;
        id->ieee[2] = 0x52;
    }

    id->mdts = n->params.mdts;
    id->ver = cpu_to_le32(NVME_SPEC_VER);
    id->oacs = cpu_to_le16(NVME_OACS_NS_MGMT | NVME_OACS_FORMAT);
    id->cntrltype = 0x1;

    /*
     * Because the controller always completes the Abort command immediately,
     * there can never be more than one concurrently executing Abort command,
     * so this value is never used for anything. Note that there can easily be
     * many Abort commands in the queues, but they are not considered
     * "executing" until processed by nvme_abort.
     *
     * The specification recommends a value of 3 for Abort Command Limit (four
     * concurrently outstanding Abort commands), so lets use that though it is
     * inconsequential.
     */
    id->acl = 3;
    id->aerl = n->params.aerl;
    id->frmw = (NVME_NUM_FW_SLOTS << 1) | NVME_FRMW_SLOT1_RO;
    id->lpa = NVME_LPA_NS_SMART | NVME_LPA_CSE | NVME_LPA_EXTENDED;

    /* recommended default value (~70 C) */
    id->wctemp = cpu_to_le16(NVME_TEMPERATURE_WARNING);
    id->cctemp = cpu_to_le16(NVME_TEMPERATURE_CRITICAL);

    id->sqes = (0x6 << 4) | 0x6;
    id->cqes = (0x4 << 4) | 0x4;
    id->nn = cpu_to_le32(NVME_MAX_NAMESPACES);
    id->oncs = cpu_to_le16(NVME_ONCS_WRITE_ZEROES | NVME_ONCS_TIMESTAMP |
                           NVME_ONCS_FEATURES | NVME_ONCS_DSM |
                           NVME_ONCS_COMPARE | NVME_ONCS_COPY);

    /*
     * NOTE: If this device ever supports a command set that does NOT use 0x0
     * as a Flush-equivalent operation, support for the broadcast NSID in Flush
     * should probably be removed.
     *
     * See comment in nvme_io_cmd.
     */
    id->vwc = NVME_VWC_NSID_BROADCAST_SUPPORT | NVME_VWC_PRESENT;

    id->ocfs = cpu_to_le16(NVME_OCFS_COPY_FORMAT_0);
    id->sgls = cpu_to_le32(NVME_CTRL_SGLS_SUPPORT_NO_ALIGN |
                           NVME_CTRL_SGLS_BITBUCKET);

    nvme_init_subnqn(n);

    id->psd[0].mp = cpu_to_le16(0x9c4);
    id->psd[0].enlat = cpu_to_le32(0x10);
    id->psd[0].exlat = cpu_to_le32(0x4);

    if (n->subsys) {
        id->cmic |= NVME_CMIC_MULTI_CTRL;
    }

    NVME_CAP_SET_MQES(cap, 0x7ff);
    NVME_CAP_SET_CQR(cap, 1);
    NVME_CAP_SET_TO(cap, 0xf);
    NVME_CAP_SET_CSS(cap, NVME_CAP_CSS_NVM);
    NVME_CAP_SET_CSS(cap, NVME_CAP_CSS_CSI_SUPP);
    NVME_CAP_SET_CSS(cap, NVME_CAP_CSS_ADMIN_ONLY);
    NVME_CAP_SET_MPSMAX(cap, 4);
    NVME_CAP_SET_CMBS(cap, n->params.cmb_size_mb ? 1 : 0);
    NVME_CAP_SET_PMRS(cap, n->pmr.dev ? 1 : 0);
    stq_le_p(&n->bar.cap, cap);

    stl_le_p(&n->bar.vs, NVME_SPEC_VER);
    n->bar.intmc = n->bar.intms = 0;
}