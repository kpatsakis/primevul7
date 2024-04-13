static uint16_t nvme_identify_ctrl_csi(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeIdentify *c = (NvmeIdentify *)&req->cmd;
    uint8_t id[NVME_IDENTIFY_DATA_SIZE] = {};
    NvmeIdCtrlNvm *id_nvm = (NvmeIdCtrlNvm *)&id;

    trace_pci_nvme_identify_ctrl_csi(c->csi);

    switch (c->csi) {
    case NVME_CSI_NVM:
        id_nvm->vsl = n->params.vsl;
        id_nvm->dmrsl = cpu_to_le32(n->dmrsl);
        break;

    case NVME_CSI_ZONED:
        ((NvmeIdCtrlZoned *)&id)->zasl = n->params.zasl;
        break;

    default:
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    return nvme_c2h(n, id, sizeof(id), req);
}