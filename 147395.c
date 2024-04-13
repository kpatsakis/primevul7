qemuProcessRecoverMigrationOut(virQEMUDriverPtr driver,
                               virDomainObjPtr vm,
                               const qemuDomainJobObj *job,
                               virDomainState state,
                               int reason,
                               unsigned int *stopFlags)
{
    qemuDomainJobPrivatePtr jobPriv = job->privateData;
    bool postcopy = state == VIR_DOMAIN_PAUSED &&
                    (reason == VIR_DOMAIN_PAUSED_POSTCOPY ||
                     reason == VIR_DOMAIN_PAUSED_POSTCOPY_FAILED);
    bool resume = false;

    switch ((qemuMigrationJobPhase) job->phase) {
    case QEMU_MIGRATION_PHASE_NONE:
    case QEMU_MIGRATION_PHASE_PREPARE:
    case QEMU_MIGRATION_PHASE_FINISH2:
    case QEMU_MIGRATION_PHASE_FINISH3:
    case QEMU_MIGRATION_PHASE_LAST:
        /* N/A for outgoing migration */
        break;

    case QEMU_MIGRATION_PHASE_BEGIN3:
        /* nothing happened so far, just forget we were about to migrate the
         * domain */
        break;

    case QEMU_MIGRATION_PHASE_PERFORM2:
    case QEMU_MIGRATION_PHASE_PERFORM3:
        /* migration is still in progress, let's cancel it and resume the
         * domain; however we can only do that before migration enters
         * post-copy mode
         */
        if (postcopy) {
            qemuMigrationAnyPostcopyFailed(driver, vm);
        } else {
            VIR_DEBUG("Cancelling unfinished migration of domain %s",
                      vm->def->name);
            if (qemuMigrationSrcCancel(driver, vm) < 0) {
                VIR_WARN("Could not cancel ongoing migration of domain %s",
                         vm->def->name);
            }
            resume = true;
        }
        break;

    case QEMU_MIGRATION_PHASE_PERFORM3_DONE:
        /* migration finished but we didn't have a chance to get the result
         * of Finish3 step; third party needs to check what to do next; in
         * post-copy mode we can use PAUSED_POSTCOPY_FAILED state for this
         */
        if (postcopy)
            qemuMigrationAnyPostcopyFailed(driver, vm);
        break;

    case QEMU_MIGRATION_PHASE_CONFIRM3_CANCELLED:
        /* Finish3 failed, we need to resume the domain, but once we enter
         * post-copy mode there's no way back, so let's just mark the domain
         * as broken in that case
         */
        if (postcopy) {
            qemuMigrationAnyPostcopyFailed(driver, vm);
        } else {
            VIR_DEBUG("Resuming domain %s after failed migration",
                      vm->def->name);
            resume = true;
        }
        break;

    case QEMU_MIGRATION_PHASE_CONFIRM3:
        /* migration completed, we need to kill the domain here */
        *stopFlags |= VIR_QEMU_PROCESS_STOP_MIGRATED;
        return -1;
    }

    if (resume) {
        /* resume the domain but only if it was paused as a result of
         * migration
         */
        if (state == VIR_DOMAIN_PAUSED &&
            (reason == VIR_DOMAIN_PAUSED_MIGRATION ||
             reason == VIR_DOMAIN_PAUSED_UNKNOWN)) {
            if (qemuProcessStartCPUs(driver, vm,
                                     VIR_DOMAIN_RUNNING_MIGRATION_CANCELED,
                                     QEMU_ASYNC_JOB_NONE) < 0) {
                VIR_WARN("Could not resume domain %s", vm->def->name);
            }
        }
    }

    qemuMigrationParamsReset(driver, vm, QEMU_ASYNC_JOB_NONE,
                             jobPriv->migParams, job->apiFlags);
    return 0;
}