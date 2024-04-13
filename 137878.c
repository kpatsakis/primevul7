static inline bool nvme_is_write(NvmeRequest *req)
{
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;

    return rw->opcode == NVME_CMD_WRITE ||
           rw->opcode == NVME_CMD_ZONE_APPEND ||
           rw->opcode == NVME_CMD_WRITE_ZEROES;
}