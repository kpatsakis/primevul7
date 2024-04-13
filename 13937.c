static void reds_mig_cleanup(RedsState *reds)
{
    if (reds->mig_inprogress) {

        if (reds->mig_wait_connect || reds->mig_wait_disconnect) {
            SpiceMigrateInterface *sif;
            spice_assert(reds->migration_interface);
            sif = SPICE_UPCAST(SpiceMigrateInterface, reds->migration_interface->base.sif);
            if (reds->mig_wait_connect) {
                sif->migrate_connect_complete(reds->migration_interface);
            } else {
                if (sif->migrate_end_complete) {
                    sif->migrate_end_complete(reds->migration_interface);
                }
            }
        }
        reds->mig_inprogress = FALSE;
        reds->mig_wait_connect = FALSE;
        reds->mig_wait_disconnect = FALSE;
        red_timer_cancel(reds->mig_timer);
        reds_mig_cleanup_wait_disconnect(reds);
    }
}