void reds_on_main_migrate_connected(RedsState *reds, int seamless)
{
    reds->src_do_seamless_migrate = seamless;
    if (reds->mig_wait_connect) {
        reds_mig_cleanup(reds);
    }
}