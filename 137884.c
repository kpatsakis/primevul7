static uint16_t nvme_rpt_empty_id_struct(NvmeCtrl *n, NvmeRequest *req)
{
    uint8_t id[NVME_IDENTIFY_DATA_SIZE] = {};

    return nvme_c2h(n, id, sizeof(id), req);
}