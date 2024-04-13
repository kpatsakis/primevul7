static void reds_mig_cleanup_wait_disconnect(RedsState *reds)
{
    reds->mig_wait_disconnect_clients.clear();
    reds->mig_wait_disconnect = FALSE;
}