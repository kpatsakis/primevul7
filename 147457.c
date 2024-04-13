qemuProcessHandleGuestCrashloaded(qemuMonitorPtr mon G_GNUC_UNUSED,
                                  virDomainObjPtr vm,
                                  void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    struct qemuProcessEvent *processEvent;

    virObjectLock(vm);
    processEvent = g_new0(struct qemuProcessEvent, 1);

    processEvent->eventType = QEMU_PROCESS_EVENT_GUEST_CRASHLOADED;
    processEvent->vm = virObjectRef(vm);

    if (virThreadPoolSendJob(driver->workerPool, 0, processEvent) < 0) {
        virObjectUnref(vm);
        qemuProcessEventFree(processEvent);
    }

    virObjectUnlock(vm);

    return 0;
}