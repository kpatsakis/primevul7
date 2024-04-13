static void reds_mig_target_client_add(RedsState *reds, RedClient *client)
{
    RedsMigTargetClient *mig_client;

    g_return_if_fail(reds);
    spice_debug("trace");
    mig_client = g_new0(RedsMigTargetClient, 1);
    mig_client->client = client;
    reds->mig_target_clients = g_list_append(reds->mig_target_clients, mig_client);
}