static uint16_t nvme_identify_cmd_set(NvmeCtrl *n, NvmeRequest *req)
{
    uint8_t list[NVME_IDENTIFY_DATA_SIZE] = {};
    static const int data_len = sizeof(list);

    trace_pci_nvme_identify_cmd_set();

    NVME_SET_CSI(*list, NVME_CSI_NVM);
    NVME_SET_CSI(*list, NVME_CSI_ZONED);

    return nvme_c2h(n, list, data_len, req);
}