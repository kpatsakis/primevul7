static uint16_t nvme_ns_attachment(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeNamespace *ns;
    NvmeCtrl *ctrl;
    uint16_t list[NVME_CONTROLLER_LIST_SIZE] = {};
    uint32_t nsid = le32_to_cpu(req->cmd.nsid);
    uint32_t dw10 = le32_to_cpu(req->cmd.cdw10);
    uint8_t sel = dw10 & 0xf;
    uint16_t *nr_ids = &list[0];
    uint16_t *ids = &list[1];
    uint16_t ret;
    int i;

    trace_pci_nvme_ns_attachment(nvme_cid(req), dw10 & 0xf);

    if (!nvme_nsid_valid(n, nsid)) {
        return NVME_INVALID_NSID | NVME_DNR;
    }

    ns = nvme_subsys_ns(n->subsys, nsid);
    if (!ns) {
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    ret = nvme_h2c(n, (uint8_t *)list, 4096, req);
    if (ret) {
        return ret;
    }

    if (!*nr_ids) {
        return NVME_NS_CTRL_LIST_INVALID | NVME_DNR;
    }

    *nr_ids = MIN(*nr_ids, NVME_CONTROLLER_LIST_SIZE - 1);
    for (i = 0; i < *nr_ids; i++) {
        ctrl = nvme_subsys_ctrl(n->subsys, ids[i]);
        if (!ctrl) {
            return NVME_NS_CTRL_LIST_INVALID | NVME_DNR;
        }

        switch (sel) {
        case NVME_NS_ATTACHMENT_ATTACH:
            if (nvme_ns(ctrl, nsid)) {
                return NVME_NS_ALREADY_ATTACHED | NVME_DNR;
            }

            if (ns->attached && !ns->params.shared) {
                return NVME_NS_PRIVATE | NVME_DNR;
            }

            nvme_attach_ns(ctrl, ns);
            nvme_select_iocs_ns(ctrl, ns);

            break;

        case NVME_NS_ATTACHMENT_DETACH:
            if (!nvme_ns(ctrl, nsid)) {
                return NVME_NS_NOT_ATTACHED | NVME_DNR;
            }

            ctrl->namespaces[nsid] = NULL;
            ns->attached--;

            nvme_update_dmrsl(ctrl);

            break;

        default:
            return NVME_INVALID_FIELD | NVME_DNR;
        }

        /*
         * Add namespace id to the changed namespace id list for event clearing
         * via Get Log Page command.
         */
        if (!test_and_set_bit(nsid, ctrl->changed_nsids)) {
            nvme_enqueue_event(ctrl, NVME_AER_TYPE_NOTICE,
                               NVME_AER_INFO_NOTICE_NS_ATTR_CHANGED,
                               NVME_LOG_CHANGED_NSLIST);
        }
    }

    return NVME_SUCCESS;
}