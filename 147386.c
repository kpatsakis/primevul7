qemuProcessHandleGuestPanic(qemuMonitorPtr mon G_GNUC_UNUSED,
                            virDomainObjPtr vm,
                            qemuMonitorEventPanicInfoPtr info,
                            void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    struct qemuProcessEvent *processEvent;

    virObjectLock(vm);
    processEvent = g_new0(struct qemuProcessEvent, 1);

    processEvent->eventType = QEMU_PROCESS_EVENT_GUESTPANIC;
    processEvent->action = vm->def->onCrash;
    processEvent->data = info;
    /* Hold an extra reference because we can't allow 'vm' to be
     * deleted before handling guest panic event is finished.
     */
    processEvent->vm = virObjectRef(vm);

    if (virThreadPoolSendJob(driver->workerPool, 0, processEvent) < 0) {
        virObjectUnref(vm);
        qemuProcessEventFree(processEvent);
    }

    virObjectUnlock(vm);

    return 0;
}