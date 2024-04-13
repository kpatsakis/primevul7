static void nvme_smart_event(NvmeCtrl *n, uint8_t event)
{
    uint8_t aer_info;

    /* Ref SPEC <Asynchronous Event Information 0x2013 SMART / Health Status> */
    if (!(NVME_AEC_SMART(n->features.async_config) & event)) {
        return;
    }

    switch (event) {
    case NVME_SMART_SPARE:
        aer_info = NVME_AER_INFO_SMART_SPARE_THRESH;
        break;
    case NVME_SMART_TEMPERATURE:
        aer_info = NVME_AER_INFO_SMART_TEMP_THRESH;
        break;
    case NVME_SMART_RELIABILITY:
    case NVME_SMART_MEDIA_READ_ONLY:
    case NVME_SMART_FAILED_VOLATILE_MEDIA:
    case NVME_SMART_PMR_UNRELIABLE:
        aer_info = NVME_AER_INFO_SMART_RELIABILITY;
        break;
    default:
        return;
    }

    nvme_enqueue_event(n, NVME_AER_TYPE_SMART, aer_info, NVME_LOG_SMART_INFO);
}