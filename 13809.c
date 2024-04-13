uint32_t reds_qxl_ram_size(RedsState *reds)
{
    QXLInstance *first;
    if (reds->qxl_instances.empty()) {
        return 0;
    }

    first = *reds->qxl_instances.begin();
    return red_qxl_get_ram_size(first);
}