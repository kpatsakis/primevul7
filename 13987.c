static void reds_mig_finished(RedsState *reds, int completed)
{
    spice_debug("trace");

    reds->mig_inprogress = TRUE;

    if (reds->src_do_seamless_migrate && completed) {
        reds_migrate_channels_seamless(reds);
    } else {
        reds->main_channel->migrate_src_complete(completed);
    }

    if (completed) {
        reds_mig_fill_wait_disconnect(reds);
    } else {
        reds_mig_cleanup(reds);
    }
    reds_mig_release(reds->config);
}