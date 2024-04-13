static uint16_t nvme_check_dulbe(NvmeNamespace *ns, uint64_t slba,
                                 uint32_t nlb)
{
    int ret;
    Error *err = NULL;

    ret = nvme_block_status_all(ns, slba, nlb, BDRV_BLOCK_DATA);
    if (ret) {
        if (ret < 0) {
            error_setg_errno(&err, -ret, "unable to get block status");
            error_report_err(err);

            return NVME_INTERNAL_DEV_ERROR;
        }

        return NVME_DULB;
    }

    return NVME_SUCCESS;
}