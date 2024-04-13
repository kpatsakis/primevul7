static void nvme_cmb_enable_regs(NvmeCtrl *n)
{
    uint32_t cmbloc = ldl_le_p(&n->bar.cmbloc);
    uint32_t cmbsz = ldl_le_p(&n->bar.cmbsz);

    NVME_CMBLOC_SET_CDPCILS(cmbloc, 1);
    NVME_CMBLOC_SET_CDPMLS(cmbloc, 1);
    NVME_CMBLOC_SET_BIR(cmbloc, NVME_CMB_BIR);
    stl_le_p(&n->bar.cmbloc, cmbloc);

    NVME_CMBSZ_SET_SQS(cmbsz, 1);
    NVME_CMBSZ_SET_CQS(cmbsz, 0);
    NVME_CMBSZ_SET_LISTS(cmbsz, 1);
    NVME_CMBSZ_SET_RDS(cmbsz, 1);
    NVME_CMBSZ_SET_WDS(cmbsz, 1);
    NVME_CMBSZ_SET_SZU(cmbsz, 2); /* MBs */
    NVME_CMBSZ_SET_SZ(cmbsz, n->params.cmb_size_mb);
    stl_le_p(&n->bar.cmbsz, cmbsz);
}