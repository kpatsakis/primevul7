qemuProcessHandleSerialChanged(qemuMonitorPtr mon G_GNUC_UNUSED,
                               virDomainObjPtr vm,
                               const char *devAlias,
                               bool connected,
                               void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    struct qemuProcessEvent *processEvent = NULL;
    char *data;

    virObjectLock(vm);

    VIR_DEBUG("Serial port %s state changed to '%d' in domain %p %s",
              devAlias, connected, vm, vm->def->name);

    processEvent = g_new0(struct qemuProcessEvent, 1);

    processEvent->eventType = QEMU_PROCESS_EVENT_SERIAL_CHANGED;
    data = g_strdup(devAlias);
    processEvent->data = data;
    processEvent->action = connected;
    processEvent->vm = virObjectRef(vm);

    if (virThreadPoolSendJob(driver->workerPool, 0, processEvent) < 0) {
        virObjectUnref(vm);
        goto error;
    }

 cleanup:
    virObjectUnlock(vm);
    return 0;
 error:
    qemuProcessEventFree(processEvent);
    goto cleanup;
}