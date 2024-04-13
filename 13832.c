SPICE_GNUC_VISIBLE int spice_server_migrate_end(SpiceServer *reds, int completed)
{
    SpiceMigrateInterface *sif;
    int ret = 0;

    spice_debug("trace");

    spice_assert(reds->migration_interface);

    sif = SPICE_UPCAST(SpiceMigrateInterface, reds->migration_interface->base.sif);
    if (completed && !reds->expect_migrate && !reds->clients.empty()) {
        spice_warning("spice_server_migrate_info was not called, disconnecting clients");
        reds_disconnect(reds);
        ret = -1;
        goto complete;
    }

    reds->expect_migrate = FALSE;
    if (!reds_main_channel_connected(reds)) {
        spice_debug("no peer connected");
        goto complete;
    }
    reds_mig_finished(reds, completed);
    return 0;
complete:
    if (sif->migrate_end_complete) {
        sif->migrate_end_complete(reds->migration_interface);
    }
    return ret;
}