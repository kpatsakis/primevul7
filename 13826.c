static RedsMigTargetClient* reds_mig_target_client_find(RedsState *reds, RedClient *client)
{
    GList *l;

    for (l = reds->mig_target_clients; l != NULL; l = l->next) {
        RedsMigTargetClient *mig_client = (RedsMigTargetClient*) l->data;

        if (mig_client->client == client) {
            return mig_client;
        }
    }
    return NULL;
}