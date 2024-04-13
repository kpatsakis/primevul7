SPICE_GNUC_VISIBLE int spice_server_migrate_switch(SpiceServer *reds)
{
    spice_debug("trace");
    if (reds->clients.empty()) {
       return 0;
    }
    reds->expect_migrate = FALSE;
    if (!reds->config->mig_spice) {
        spice_warning("spice_server_migrate_switch called without migrate_info set");
        return 0;
    }
    reds->main_channel->migrate_switch(reds->config->mig_spice);
    reds_mig_release(reds->config);
    return 0;
}