void reds_on_client_semi_seamless_migrate_complete(RedsState *reds, RedClient *client)
{
    MainChannelClient *mcc;

    spice_debug("%p", client);
    mcc = client->get_main();

    // TODO: not doing net test. consider doing it on client_migrate_info
    mcc->push_init(reds->qxl_instances.size(), reds->mouse_mode,
                   reds->is_client_mouse_allowed,
                   reds_get_mm_time() - MM_TIME_DELTA,
                   reds_qxl_ram_size(reds));
    reds_link_mig_target_channels(reds, client);
    mcc->migrate_dst_complete();
}