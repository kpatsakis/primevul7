qemuProcessRecoverJob(virQEMUDriverPtr driver,
                      virDomainObjPtr vm,
                      const qemuDomainJobObj *job,
                      unsigned int *stopFlags)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    virDomainState state;
    int reason;
    unsigned long long now;

    state = virDomainObjGetState(vm, &reason);

    switch (job->asyncJob) {
    case QEMU_ASYNC_JOB_MIGRATION_OUT:
        if (qemuProcessRecoverMigrationOut(driver, vm, job,
                                           state, reason, stopFlags) < 0)
            return -1;
        break;

    case QEMU_ASYNC_JOB_MIGRATION_IN:
        if (qemuProcessRecoverMigrationIn(driver, vm, job,
                                          state, reason) < 0)
            return -1;
        break;

    case QEMU_ASYNC_JOB_SAVE:
    case QEMU_ASYNC_JOB_DUMP:
    case QEMU_ASYNC_JOB_SNAPSHOT:
        qemuDomainObjEnterMonitor(driver, vm);
        ignore_value(qemuMonitorMigrateCancel(priv->mon));
        if (qemuDomainObjExitMonitor(driver, vm) < 0)
            return -1;
        /* resume the domain but only if it was paused as a result of
         * running a migration-to-file operation.  Although we are
         * recovering an async job, this function is run at startup
         * and must resume things using sync monitor connections.  */
         if (state == VIR_DOMAIN_PAUSED &&
             ((job->asyncJob == QEMU_ASYNC_JOB_DUMP &&
               reason == VIR_DOMAIN_PAUSED_DUMP) ||
              (job->asyncJob == QEMU_ASYNC_JOB_SAVE &&
               reason == VIR_DOMAIN_PAUSED_SAVE) ||
              (job->asyncJob == QEMU_ASYNC_JOB_SNAPSHOT &&
               (reason == VIR_DOMAIN_PAUSED_SNAPSHOT ||
                reason == VIR_DOMAIN_PAUSED_MIGRATION)) ||
              reason == VIR_DOMAIN_PAUSED_UNKNOWN)) {
             if (qemuProcessStartCPUs(driver, vm,
                                      VIR_DOMAIN_RUNNING_SAVE_CANCELED,
                                      QEMU_ASYNC_JOB_NONE) < 0) {
                 VIR_WARN("Could not resume domain '%s' after migration to file",
                          vm->def->name);
            }
        }
        break;

    case QEMU_ASYNC_JOB_START:
        /* Already handled in VIR_DOMAIN_PAUSED_STARTING_UP check. */
        break;

    case QEMU_ASYNC_JOB_BACKUP:
        ignore_value(virTimeMillisNow(&now));

        /* Restore the config of the async job which is not persisted */
        priv->jobs_queued++;
        priv->job.asyncJob = QEMU_ASYNC_JOB_BACKUP;
        priv->job.asyncOwnerAPI = virThreadJobGet();
        priv->job.asyncStarted = now;

        qemuDomainObjSetAsyncJobMask(vm, (QEMU_JOB_DEFAULT_MASK |
                                          JOB_MASK(QEMU_JOB_SUSPEND) |
                                          JOB_MASK(QEMU_JOB_MODIFY)));

        /* We reset the job parameters for backup so that the job will look
         * active. This is possible because we are able to recover the state
         * of blockjobs and also the backup job allows all sub-job types */
        priv->job.current = g_new0(qemuDomainJobInfo, 1);
        priv->job.current->operation = VIR_DOMAIN_JOB_OPERATION_BACKUP;
        priv->job.current->statsType = QEMU_DOMAIN_JOB_STATS_TYPE_BACKUP;
        priv->job.current->status = QEMU_DOMAIN_JOB_STATUS_ACTIVE;
        priv->job.current->started = now;
        break;

    case QEMU_ASYNC_JOB_NONE:
    case QEMU_ASYNC_JOB_LAST:
        break;
    }

    if (!virDomainObjIsActive(vm))
        return -1;

    /* In case any special handling is added for job type that has been ignored
     * before, QEMU_DOMAIN_TRACK_JOBS (from qemu_domain.h) needs to be updated
     * for the job to be properly tracked in domain state XML.
     */
    switch (job->active) {
    case QEMU_JOB_QUERY:
        /* harmless */
        break;

    case QEMU_JOB_DESTROY:
        VIR_DEBUG("Domain %s should have already been destroyed",
                  vm->def->name);
        return -1;

    case QEMU_JOB_SUSPEND:
        /* mostly harmless */
        break;

    case QEMU_JOB_MODIFY:
        /* XXX depending on the command we may be in an inconsistent state and
         * we should probably fall back to "monitor error" state and refuse to
         */
        break;

    case QEMU_JOB_MIGRATION_OP:
    case QEMU_JOB_ABORT:
    case QEMU_JOB_ASYNC:
    case QEMU_JOB_ASYNC_NESTED:
        /* async job was already handled above */
    case QEMU_JOB_NONE:
    case QEMU_JOB_LAST:
        break;
    }

    return 0;
}