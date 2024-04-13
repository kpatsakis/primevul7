qemuProcessHandleWatchdog(qemuMonitorPtr mon G_GNUC_UNUSED,
                          virDomainObjPtr vm,
                          int action,
                          void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr watchdogEvent = NULL;
    virObjectEventPtr lifecycleEvent = NULL;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);

    virObjectLock(vm);
    watchdogEvent = virDomainEventWatchdogNewFromObj(vm, action);

    if (action == VIR_DOMAIN_EVENT_WATCHDOG_PAUSE &&
        virDomainObjGetState(vm, NULL) == VIR_DOMAIN_RUNNING) {
        qemuDomainObjPrivatePtr priv = vm->privateData;
        VIR_DEBUG("Transitioned guest %s to paused state due to watchdog", vm->def->name);

        virDomainObjSetState(vm, VIR_DOMAIN_PAUSED, VIR_DOMAIN_PAUSED_WATCHDOG);
        lifecycleEvent = virDomainEventLifecycleNewFromObj(vm,
                                                  VIR_DOMAIN_EVENT_SUSPENDED,
                                                  VIR_DOMAIN_EVENT_SUSPENDED_WATCHDOG);

        VIR_FREE(priv->lockState);
        if (virDomainLockProcessPause(driver->lockManager, vm, &priv->lockState) < 0)
            VIR_WARN("Unable to release lease on %s", vm->def->name);
        VIR_DEBUG("Preserving lock state '%s'", NULLSTR(priv->lockState));

        if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0) {
            VIR_WARN("Unable to save status on vm %s after watchdog event",
                     vm->def->name);
        }
    }

    if (vm->def->watchdog->action == VIR_DOMAIN_WATCHDOG_ACTION_DUMP) {
        struct qemuProcessEvent *processEvent;
        processEvent = g_new0(struct qemuProcessEvent, 1);

        processEvent->eventType = QEMU_PROCESS_EVENT_WATCHDOG;
        processEvent->action = VIR_DOMAIN_WATCHDOG_ACTION_DUMP;
        /* Hold an extra reference because we can't allow 'vm' to be
         * deleted before handling watchdog event is finished.
         */
        processEvent->vm = virObjectRef(vm);
        if (virThreadPoolSendJob(driver->workerPool, 0, processEvent) < 0) {
            virObjectUnref(vm);
            qemuProcessEventFree(processEvent);
        }
    }

    virObjectUnlock(vm);
    virObjectEventStateQueue(driver->domainEventState, watchdogEvent);
    virObjectEventStateQueue(driver->domainEventState, lifecycleEvent);

    return 0;
}