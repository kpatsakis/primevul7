static void nvme_format_set(NvmeNamespace *ns, NvmeCmd *cmd)
{
    uint32_t dw10 = le32_to_cpu(cmd->cdw10);
    uint8_t lbaf = dw10 & 0xf;
    uint8_t pi = (dw10 >> 5) & 0x7;
    uint8_t mset = (dw10 >> 4) & 0x1;
    uint8_t pil = (dw10 >> 8) & 0x1;

    trace_pci_nvme_format_set(ns->params.nsid, lbaf, mset, pi, pil);

    ns->id_ns.dps = (pil << 3) | pi;
    ns->id_ns.flbas = lbaf | (mset << 4);

    nvme_ns_init_format(ns);
}