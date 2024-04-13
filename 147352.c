qemuProcessHandleMigrationStatus(qemuMonitorPtr mon G_GNUC_UNUSED,
                                 virDomainObjPtr vm,
                                 int status,
                                 void *opaque)
{
    qemuDomainObjPrivatePtr priv;
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr event = NULL;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);
    int reason;

    virObjectLock(vm);

    VIR_DEBUG("Migration of domain %p %s changed state to %s",
              vm, vm->def->name,
              qemuMonitorMigrationStatusTypeToString(status));

    priv = vm->privateData;
    if (priv->job.asyncJob == QEMU_ASYNC_JOB_NONE) {
        VIR_DEBUG("got MIGRATION event without a migration job");
        goto cleanup;
    }

    priv->job.current->stats.mig.status = status;
    virDomainObjBroadcast(vm);

    if (status == QEMU_MONITOR_MIGRATION_STATUS_POSTCOPY &&
        priv->job.asyncJob == QEMU_ASYNC_JOB_MIGRATION_OUT &&
        virDomainObjGetState(vm, &reason) == VIR_DOMAIN_PAUSED &&
        reason == VIR_DOMAIN_PAUSED_MIGRATION) {
        VIR_DEBUG("Correcting paused state reason for domain %s to %s",
                  vm->def->name,
                  virDomainPausedReasonTypeToString(VIR_DOMAIN_PAUSED_POSTCOPY));

        virDomainObjSetState(vm, VIR_DOMAIN_PAUSED, VIR_DOMAIN_PAUSED_POSTCOPY);
        event = virDomainEventLifecycleNewFromObj(vm,
                                                  VIR_DOMAIN_EVENT_SUSPENDED,
                                                  VIR_DOMAIN_EVENT_SUSPENDED_POSTCOPY);

        if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0) {
            VIR_WARN("Unable to save status on vm %s after state change",
                     vm->def->name);
        }
    }

 cleanup:
    virObjectUnlock(vm);
    virObjectEventStateQueue(driver->domainEventState, event);
    return 0;
}