qemuProcessHandleStop(qemuMonitorPtr mon G_GNUC_UNUSED,
                      virDomainObjPtr vm,
                      void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr event = NULL;
    virDomainPausedReason reason;
    virDomainEventSuspendedDetailType detail;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);
    qemuDomainObjPrivatePtr priv = vm->privateData;

    virObjectLock(vm);

    reason = priv->pausedReason;
    priv->pausedReason = VIR_DOMAIN_PAUSED_UNKNOWN;

    /* In case of fake reboot qemu paused state is transient so don't
     * reveal it in domain state nor sent events */
    if (virDomainObjGetState(vm, NULL) == VIR_DOMAIN_RUNNING &&
        !priv->pausedShutdown) {
        if (priv->job.asyncJob == QEMU_ASYNC_JOB_MIGRATION_OUT) {
            if (priv->job.current->status == QEMU_DOMAIN_JOB_STATUS_POSTCOPY)
                reason = VIR_DOMAIN_PAUSED_POSTCOPY;
            else
                reason = VIR_DOMAIN_PAUSED_MIGRATION;
        }

        detail = qemuDomainPausedReasonToSuspendedEvent(reason);
        VIR_DEBUG("Transitioned guest %s to paused state, "
                  "reason %s, event detail %d",
                  vm->def->name, virDomainPausedReasonTypeToString(reason),
                  detail);

        if (priv->job.current)
            ignore_value(virTimeMillisNow(&priv->job.current->stopped));

        if (priv->signalStop)
            virDomainObjBroadcast(vm);

        virDomainObjSetState(vm, VIR_DOMAIN_PAUSED, reason);
        event = virDomainEventLifecycleNewFromObj(vm,
                                                  VIR_DOMAIN_EVENT_SUSPENDED,
                                                  detail);

        VIR_FREE(priv->lockState);
        if (virDomainLockProcessPause(driver->lockManager, vm, &priv->lockState) < 0)
            VIR_WARN("Unable to release lease on %s", vm->def->name);
        VIR_DEBUG("Preserving lock state '%s'", NULLSTR(priv->lockState));

        if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0) {
            VIR_WARN("Unable to save status on vm %s after state change",
                     vm->def->name);
        }
    }

    virObjectUnlock(vm);
    virObjectEventStateQueue(driver->domainEventState, event);

    return 0;
}