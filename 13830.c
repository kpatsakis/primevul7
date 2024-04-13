static void reds_mig_fill_wait_disconnect(RedsState *reds)
{
    spice_assert(!reds->clients.empty());
    /* tracking the clients, in order to ignore disconnection
     * of clients that got connected to the src after migration completion.*/
    for (auto client: reds->clients) {
        reds->mig_wait_disconnect_clients.push_front(client);
    }
    reds->mig_wait_connect = FALSE;
    reds->mig_wait_disconnect = TRUE;
    red_timer_start(reds->mig_timer, MIGRATE_TIMEOUT);
}