static uint16_t nvme_identify_ctrl_list(NvmeCtrl *n, NvmeRequest *req,
                                        bool attached)
{
    NvmeIdentify *c = (NvmeIdentify *)&req->cmd;
    uint32_t nsid = le32_to_cpu(c->nsid);
    uint16_t min_id = le16_to_cpu(c->ctrlid);
    uint16_t list[NVME_CONTROLLER_LIST_SIZE] = {};
    uint16_t *ids = &list[1];
    NvmeNamespace *ns;
    NvmeCtrl *ctrl;
    int cntlid, nr_ids = 0;

    trace_pci_nvme_identify_ctrl_list(c->cns, min_id);

    if (!n->subsys) {
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    if (attached) {
        if (nsid == NVME_NSID_BROADCAST) {
            return NVME_INVALID_FIELD | NVME_DNR;
        }

        ns = nvme_subsys_ns(n->subsys, nsid);
        if (!ns) {
            return NVME_INVALID_FIELD | NVME_DNR;
        }
    }

    for (cntlid = min_id; cntlid < ARRAY_SIZE(n->subsys->ctrls); cntlid++) {
        ctrl = nvme_subsys_ctrl(n->subsys, cntlid);
        if (!ctrl) {
            continue;
        }

        if (attached && !nvme_ns(ctrl, nsid)) {
            continue;
        }

        ids[nr_ids++] = cntlid;
    }

    list[0] = nr_ids;

    return nvme_c2h(n, (uint8_t *)list, sizeof(list), req);
}