static void reds_mig_remove_wait_disconnect_client(RedsState *reds, RedClient *client)
{
    auto &clients(reds->mig_wait_disconnect_clients);
    g_warn_if_fail(std::find(clients.begin(), clients.end(), client) != clients.end());

    clients.remove(client);
    if (clients.empty()) {
       reds_mig_cleanup(reds);
    }
}