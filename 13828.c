static void reds_disconnect(RedsState *reds)
{
    spice_debug("trace");
    for (auto client: reds->clients) {
        reds_client_disconnect(reds, client);
    }
    reds_mig_cleanup(reds);
}