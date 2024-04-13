qemuProcessRecoverMigrationIn(virQEMUDriverPtr driver,
                              virDomainObjPtr vm,
                              const qemuDomainJobObj *job,
                              virDomainState state,
                              int reason)
{

    qemuDomainJobPrivatePtr jobPriv = job->privateData;
    bool postcopy = (state == VIR_DOMAIN_PAUSED &&
                     reason == VIR_DOMAIN_PAUSED_POSTCOPY_FAILED) ||
                    (state == VIR_DOMAIN_RUNNING &&
                     reason == VIR_DOMAIN_RUNNING_POSTCOPY);

    switch ((qemuMigrationJobPhase) job->phase) {
    case QEMU_MIGRATION_PHASE_NONE:
    case QEMU_MIGRATION_PHASE_PERFORM2:
    case QEMU_MIGRATION_PHASE_BEGIN3:
    case QEMU_MIGRATION_PHASE_PERFORM3:
    case QEMU_MIGRATION_PHASE_PERFORM3_DONE:
    case QEMU_MIGRATION_PHASE_CONFIRM3_CANCELLED:
    case QEMU_MIGRATION_PHASE_CONFIRM3:
    case QEMU_MIGRATION_PHASE_LAST:
        /* N/A for incoming migration */
        break;

    case QEMU_MIGRATION_PHASE_PREPARE:
        VIR_DEBUG("Killing unfinished incoming migration for domain %s",
                  vm->def->name);
        return -1;

    case QEMU_MIGRATION_PHASE_FINISH2:
        /* source domain is already killed so let's just resume the domain
         * and hope we are all set */
        VIR_DEBUG("Incoming migration finished, resuming domain %s",
                  vm->def->name);
        if (qemuProcessStartCPUs(driver, vm,
                                 VIR_DOMAIN_RUNNING_MIGRATED,
                                 QEMU_ASYNC_JOB_NONE) < 0) {
            VIR_WARN("Could not resume domain %s", vm->def->name);
        }
        break;

    case QEMU_MIGRATION_PHASE_FINISH3:
        /* migration finished, we started resuming the domain but didn't
         * confirm success or failure yet; killing it seems safest unless
         * we already started guest CPUs or we were in post-copy mode */
        if (postcopy) {
            qemuMigrationAnyPostcopyFailed(driver, vm);
        } else if (state != VIR_DOMAIN_RUNNING) {
            VIR_DEBUG("Killing migrated domain %s", vm->def->name);
            return -1;
        }
        break;
    }

    qemuMigrationParamsReset(driver, vm, QEMU_ASYNC_JOB_NONE,
                             jobPriv->migParams, job->apiFlags);
    return 0;
}