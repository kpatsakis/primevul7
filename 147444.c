qemuProcessHandlePRManagerStatusChanged(qemuMonitorPtr mon G_GNUC_UNUSED,
                                        virDomainObjPtr vm,
                                        const char *prManager,
                                        bool connected,
                                        void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    qemuDomainObjPrivatePtr priv;
    struct qemuProcessEvent *processEvent = NULL;
    const char *managedAlias = qemuDomainGetManagedPRAlias();
    int ret = -1;

    virObjectLock(vm);

    VIR_DEBUG("pr-manager %s status changed for domain %p %s connected=%d",
              prManager, vm, vm->def->name, connected);

    if (connected) {
        /* Connect events are boring. */
        ret = 0;
        goto cleanup;
    }
    /* Disconnect events are more interesting. */

    if (STRNEQ(prManager, managedAlias)) {
        VIR_DEBUG("pr-manager %s not managed, ignoring event",
                  prManager);
        ret = 0;
        goto cleanup;
    }

    priv = vm->privateData;
    priv->prDaemonRunning = false;

    processEvent = g_new0(struct qemuProcessEvent, 1);

    processEvent->eventType = QEMU_PROCESS_EVENT_PR_DISCONNECT;
    processEvent->vm = virObjectRef(vm);

    if (virThreadPoolSendJob(driver->workerPool, 0, processEvent) < 0) {
        qemuProcessEventFree(processEvent);
        virObjectUnref(vm);
        goto cleanup;
    }

    ret = 0;
 cleanup:
    virObjectUnlock(vm);
    return ret;
}