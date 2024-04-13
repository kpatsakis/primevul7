static uint16_t nvme_identify_ns(NvmeCtrl *n, NvmeRequest *req, bool active)
{
    NvmeNamespace *ns;
    NvmeIdentify *c = (NvmeIdentify *)&req->cmd;
    uint32_t nsid = le32_to_cpu(c->nsid);

    trace_pci_nvme_identify_ns(nsid);

    if (!nvme_nsid_valid(n, nsid) || nsid == NVME_NSID_BROADCAST) {
        return NVME_INVALID_NSID | NVME_DNR;
    }

    ns = nvme_ns(n, nsid);
    if (unlikely(!ns)) {
        if (!active) {
            ns = nvme_subsys_ns(n->subsys, nsid);
            if (!ns) {
                return nvme_rpt_empty_id_struct(n, req);
            }
        } else {
            return nvme_rpt_empty_id_struct(n, req);
        }
    }

    if (active || ns->csi == NVME_CSI_NVM) {
        return nvme_c2h(n, (uint8_t *)&ns->id_ns, sizeof(NvmeIdNs), req);
    }

    return NVME_INVALID_CMD_SET | NVME_DNR;
}