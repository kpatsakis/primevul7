static void reds_client_monitors_config(RedsState *reds, VDAgentMonitorsConfig *monitors_config)
{
    FOREACH_QXL_INSTANCE(reds, qxl) {
        if (!red_qxl_client_monitors_config(qxl, monitors_config)) {
            /* this is a normal condition, some qemu devices might not implement it */
            spice_debug("QXLInterface::client_monitors_config failed");
        }
    }
}