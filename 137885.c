static uint16_t nvme_identify_ns_descr_list(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeNamespace *ns;
    NvmeIdentify *c = (NvmeIdentify *)&req->cmd;
    uint32_t nsid = le32_to_cpu(c->nsid);
    uint8_t list[NVME_IDENTIFY_DATA_SIZE] = {};
    uint8_t *pos = list;
    struct {
        NvmeIdNsDescr hdr;
        uint8_t v[NVME_NIDL_UUID];
    } QEMU_PACKED uuid = {};
    struct {
        NvmeIdNsDescr hdr;
        uint64_t v;
    } QEMU_PACKED eui64 = {};
    struct {
        NvmeIdNsDescr hdr;
        uint8_t v;
    } QEMU_PACKED csi = {};

    trace_pci_nvme_identify_ns_descr_list(nsid);

    if (!nvme_nsid_valid(n, nsid) || nsid == NVME_NSID_BROADCAST) {
        return NVME_INVALID_NSID | NVME_DNR;
    }

    ns = nvme_ns(n, nsid);
    if (unlikely(!ns)) {
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    /*
     * If the EUI-64 field is 0 and the NGUID field is 0, the namespace must
     * provide a valid Namespace UUID in the Namespace Identification Descriptor
     * data structure. QEMU does not yet support setting NGUID.
     */
    uuid.hdr.nidt = NVME_NIDT_UUID;
    uuid.hdr.nidl = NVME_NIDL_UUID;
    memcpy(uuid.v, ns->params.uuid.data, NVME_NIDL_UUID);
    memcpy(pos, &uuid, sizeof(uuid));
    pos += sizeof(uuid);

    if (ns->params.eui64) {
        eui64.hdr.nidt = NVME_NIDT_EUI64;
        eui64.hdr.nidl = NVME_NIDL_EUI64;
        eui64.v = cpu_to_be64(ns->params.eui64);
        memcpy(pos, &eui64, sizeof(eui64));
        pos += sizeof(eui64);
    }

    csi.hdr.nidt = NVME_NIDT_CSI;
    csi.hdr.nidl = NVME_NIDL_CSI;
    csi.v = ns->csi;
    memcpy(pos, &csi, sizeof(csi));
    pos += sizeof(csi);

    return nvme_c2h(n, list, sizeof(list), req);
}