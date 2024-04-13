static gboolean reds_use_client_monitors_config(RedsState *reds)
{
    if (reds->qxl_instances.empty()) {
        return FALSE;
    }

    FOREACH_QXL_INSTANCE(reds, qxl) {
        if (!red_qxl_client_monitors_config(qxl, NULL))
            return FALSE;
    }
    return TRUE;
}