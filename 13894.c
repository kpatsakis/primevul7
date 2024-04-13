static int do_spice_init(RedsState *reds, SpiceCoreInterface *core_interface)
{
    spice_debug("starting %s", VERSION);

    if (core_interface->base.major_version != SPICE_INTERFACE_CORE_MAJOR) {
        spice_warning("bad core interface version");
        goto err;
    }
    reds->core = core_interface_adapter;
    reds->core.public_interface = core_interface;
    reds->agent_dev = red::make_shared<RedCharDeviceVDIPort>(reds);
    reds_update_agent_properties(reds);
    reds->main_dispatcher = red::make_shared<MainDispatcher>(reds);
    reds->mig_target_clients = NULL;
    reds->vm_running = TRUE; /* for backward compatibility */

    if (!(reds->mig_timer = reds->core.timer_new(migrate_timeout, reds))) {
        spice_error("migration timer create failed");
    }
    /* Note that this will not actually send the mm_time to the client because
     * the main channel is not connected yet. This would have been redundant
     * with the RED_PIPE_ITEM_TYPE_MAIN_INIT message anyway.
     */
    reds_enable_mm_time(reds);

    if (reds_init_net(reds) < 0) {
        spice_warning("Failed to open SPICE sockets");
        goto err;
    }
    if (reds->secure_listen_socket != -1) {
        if (reds_init_ssl(reds) < 0) {
            goto err;
        }
    }
#if HAVE_SASL
    int saslerr;
    if ((saslerr = sasl_server_init(NULL, reds->config->sasl_appname ?
                                    reds->config->sasl_appname : "spice")) != SASL_OK) {
        spice_error("Failed to initialize SASL auth %s",
                  sasl_errstring(saslerr, NULL, NULL));
        goto err;
    }
#endif

    reds->main_channel = main_channel_new(reds);
    reds->inputs_channel = inputs_channel_new(reds);

    reds->mouse_mode = SPICE_MOUSE_MODE_SERVER;

    spice_buffer_free(&reds->client_monitors_config);

    reds->allow_multiple_clients = getenv(SPICE_DEBUG_ALLOW_MC_ENV) != NULL;
    if (reds->allow_multiple_clients) {
        spice_warning("spice: allowing multiple client connections");
    }
    pthread_mutex_lock(&global_reds_lock);
    servers = g_list_prepend(servers, reds);
    pthread_mutex_unlock(&global_reds_lock);
    return 0;

err:
    reds_cleanup_net(reds);
    return -1;
}