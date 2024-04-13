static uint16_t nvme_map_mptr(NvmeCtrl *n, NvmeSg *sg, size_t len,
                              NvmeCmd *cmd)
{
    int psdt = NVME_CMD_FLAGS_PSDT(cmd->flags);
    hwaddr mptr = le64_to_cpu(cmd->mptr);
    uint16_t status;

    if (psdt == NVME_PSDT_SGL_MPTR_SGL) {
        NvmeSglDescriptor sgl;

        if (nvme_addr_read(n, mptr, &sgl, sizeof(sgl))) {
            return NVME_DATA_TRAS_ERROR;
        }

        status = nvme_map_sgl(n, sg, sgl, len, cmd);
        if (status && (status & 0x7ff) == NVME_DATA_SGL_LEN_INVALID) {
            status = NVME_MD_SGL_LEN_INVALID | NVME_DNR;
        }

        return status;
    }

    nvme_sg_init(n, sg, nvme_addr_is_dma(n, mptr));
    status = nvme_map_addr(n, sg, mptr, len);
    if (status) {
        nvme_sg_unmap(sg);
    }

    return status;
}