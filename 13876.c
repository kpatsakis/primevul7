static void reds_mig_started(RedsState *reds)
{
    spice_debug("trace");
    spice_assert(reds->config->mig_spice);

    reds->mig_inprogress = TRUE;
    reds->mig_wait_connect = TRUE;
    red_timer_start(reds->mig_timer, MIGRATE_TIMEOUT);
}