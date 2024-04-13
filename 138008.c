static uint16_t nvme_format_check(NvmeNamespace *ns, uint8_t lbaf, uint8_t pi)
{
    if (ns->params.zoned) {
        return NVME_INVALID_FORMAT | NVME_DNR;
    }

    if (lbaf > ns->id_ns.nlbaf) {
        return NVME_INVALID_FORMAT | NVME_DNR;
    }

    if (pi && (ns->id_ns.lbaf[lbaf].ms < sizeof(NvmeDifTuple))) {
        return NVME_INVALID_FORMAT | NVME_DNR;
    }

    if (pi && pi > NVME_ID_NS_DPS_TYPE_3) {
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    return NVME_SUCCESS;
}