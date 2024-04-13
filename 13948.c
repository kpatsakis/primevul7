SPICE_GNUC_VISIBLE int spice_server_migrate_connect(SpiceServer *reds, const char* dest,
                                                    int port, int secure_port,
                                                    const char* cert_subject)
{
    SpiceMigrateInterface *sif;
    int try_seamless;

    spice_debug("trace");
    spice_assert(reds->migration_interface);

    if (reds->expect_migrate) {
        spice_debug("consecutive calls without migration. Canceling previous call");
        reds->main_channel->migrate_src_complete(FALSE);
    }

    sif = SPICE_UPCAST(SpiceMigrateInterface, reds->migration_interface->base.sif);

    if (!reds_set_migration_dest_info(reds, dest, port, secure_port, cert_subject)) {
        sif->migrate_connect_complete(reds->migration_interface);
        return -1;
    }

    reds->expect_migrate = TRUE;

    /*
     * seamless migration support was added to the client after the support in
     * agent_connect_tokens, so there shouldn't be contradicition - if
     * the client is capable of seamless migration, it is capbable of agent_connected_tokens.
     * The demand for agent_connected_tokens support is in order to assure that if migration
     * occured when the agent was not connected, the tokens state after migration will still
     * be valid (see reds_reset_vdp for more details).
     */
    try_seamless = reds->seamless_migration_enabled &&
                   reds->main_channel->test_remote_cap(SPICE_MAIN_CAP_AGENT_CONNECTED_TOKENS);
    /* main channel will take care of clients that are still during migration (at target)*/
    if (reds->main_channel->migrate_connect(reds->config->mig_spice, try_seamless)) {
        reds_mig_started(reds);
    } else {
        if (reds->clients.empty()) {
            reds_mig_release(reds->config);
            spice_debug("no client connected");
        }
        sif->migrate_connect_complete(reds->migration_interface);
    }

    return 0;
}