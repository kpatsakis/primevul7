static void reds_mig_target_client_disconnect_all(RedsState *reds)
{
    RedsMigTargetClient *mig_client;

    GLIST_FOREACH(reds->mig_target_clients, RedsMigTargetClient, mig_client) {
        reds_client_disconnect(reds, mig_client->client);
    }
}