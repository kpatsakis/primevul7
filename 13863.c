void reds_on_client_seamless_migrate_complete(RedsState *reds, RedClient *client)
{
    spice_debug("trace");
    if (!reds_find_client(reds, client)) {
        spice_debug("client no longer exists");
        return;
    }
    client->get_main()->migrate_dst_complete();
}