static void reds_migrate_channels_seamless(RedsState *reds)
{
    RedClient *client;

    /* seamless migration is supported for only one client for now */
    client = reds_get_client(reds);
    client->migrate();
}