static void reds_mig_disconnect(RedsState *reds)
{
    if (reds_main_channel_connected(reds)) {
        reds_disconnect(reds);
    } else {
        reds_mig_cleanup(reds);
    }
}