static void reds_mig_target_client_free(RedsState *reds, RedsMigTargetClient *mig_client)
{
    reds->mig_target_clients = g_list_remove(reds->mig_target_clients, mig_client);
    g_list_free_full(mig_client->pending_links, g_free);
    g_free(mig_client);
}