static uint16_t nvme_identify(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeIdentify *c = (NvmeIdentify *)&req->cmd;

    trace_pci_nvme_identify(nvme_cid(req), c->cns, le16_to_cpu(c->ctrlid),
                            c->csi);

    switch (c->cns) {
    case NVME_ID_CNS_NS:
        return nvme_identify_ns(n, req, true);
    case NVME_ID_CNS_NS_PRESENT:
        return nvme_identify_ns(n, req, false);
    case NVME_ID_CNS_NS_ATTACHED_CTRL_LIST:
        return nvme_identify_ctrl_list(n, req, true);
    case NVME_ID_CNS_CTRL_LIST:
        return nvme_identify_ctrl_list(n, req, false);
    case NVME_ID_CNS_CS_NS:
        return nvme_identify_ns_csi(n, req, true);
    case NVME_ID_CNS_CS_NS_PRESENT:
        return nvme_identify_ns_csi(n, req, false);
    case NVME_ID_CNS_CTRL:
        return nvme_identify_ctrl(n, req);
    case NVME_ID_CNS_CS_CTRL:
        return nvme_identify_ctrl_csi(n, req);
    case NVME_ID_CNS_NS_ACTIVE_LIST:
        return nvme_identify_nslist(n, req, true);
    case NVME_ID_CNS_NS_PRESENT_LIST:
        return nvme_identify_nslist(n, req, false);
    case NVME_ID_CNS_CS_NS_ACTIVE_LIST:
        return nvme_identify_nslist_csi(n, req, true);
    case NVME_ID_CNS_CS_NS_PRESENT_LIST:
        return nvme_identify_nslist_csi(n, req, false);
    case NVME_ID_CNS_NS_DESCR_LIST:
        return nvme_identify_ns_descr_list(n, req);
    case NVME_ID_CNS_IO_COMMAND_SET:
        return nvme_identify_cmd_set(n, req);
    default:
        trace_pci_nvme_err_invalid_identify_cns(le32_to_cpu(c->cns));
        return NVME_INVALID_FIELD | NVME_DNR;
    }
}