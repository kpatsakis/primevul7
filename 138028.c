static uint16_t nvme_identify_nslist(NvmeCtrl *n, NvmeRequest *req,
                                     bool active)
{
    NvmeNamespace *ns;
    NvmeIdentify *c = (NvmeIdentify *)&req->cmd;
    uint32_t min_nsid = le32_to_cpu(c->nsid);
    uint8_t list[NVME_IDENTIFY_DATA_SIZE] = {};
    static const int data_len = sizeof(list);
    uint32_t *list_ptr = (uint32_t *)list;
    int i, j = 0;

    trace_pci_nvme_identify_nslist(min_nsid);

    /*
     * Both FFFFFFFFh (NVME_NSID_BROADCAST) and FFFFFFFFEh are invalid values
     * since the Active Namespace ID List should return namespaces with ids
     * *higher* than the NSID specified in the command. This is also specified
     * in the spec (NVM Express v1.3d, Section 5.15.4).
     */
    if (min_nsid >= NVME_NSID_BROADCAST - 1) {
        return NVME_INVALID_NSID | NVME_DNR;
    }

    for (i = 1; i <= NVME_MAX_NAMESPACES; i++) {
        ns = nvme_ns(n, i);
        if (!ns) {
            if (!active) {
                ns = nvme_subsys_ns(n->subsys, i);
                if (!ns) {
                    continue;
                }
            } else {
                continue;
            }
        }
        if (ns->params.nsid <= min_nsid) {
            continue;
        }
        list_ptr[j++] = cpu_to_le32(ns->params.nsid);
        if (j == data_len / sizeof(uint32_t)) {
            break;
        }
    }

    return nvme_c2h(n, list, data_len, req);
}