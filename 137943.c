static void nvme_aio_err(NvmeRequest *req, int ret)
{
    uint16_t status = NVME_SUCCESS;
    Error *local_err = NULL;

    switch (req->cmd.opcode) {
    case NVME_CMD_READ:
        status = NVME_UNRECOVERED_READ;
        break;
    case NVME_CMD_FLUSH:
    case NVME_CMD_WRITE:
    case NVME_CMD_WRITE_ZEROES:
    case NVME_CMD_ZONE_APPEND:
        status = NVME_WRITE_FAULT;
        break;
    default:
        status = NVME_INTERNAL_DEV_ERROR;
        break;
    }

    trace_pci_nvme_err_aio(nvme_cid(req), strerror(-ret), status);

    error_setg_errno(&local_err, -ret, "aio failed");
    error_report_err(local_err);

    /*
     * Set the command status code to the first encountered error but allow a
     * subsequent Internal Device Error to trump it.
     */
    if (req->status && status != NVME_INTERNAL_DEV_ERROR) {
        return;
    }

    req->status = status;
}