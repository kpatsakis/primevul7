qemuProcessHandleRdmaGidStatusChanged(qemuMonitorPtr mon G_GNUC_UNUSED,
                                      virDomainObjPtr vm,
                                      const char *netdev,
                                      bool gid_status,
                                      unsigned long long subnet_prefix,
                                      unsigned long long interface_id,
                                      void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    struct qemuProcessEvent *processEvent = NULL;
    qemuMonitorRdmaGidStatusPtr info = NULL;
    int ret = -1;

    virObjectLock(vm);

    VIR_DEBUG("netdev=%s,gid_status=%d,subnet_prefix=0x%llx,interface_id=0x%llx",
              netdev, gid_status, subnet_prefix, interface_id);

    info = g_new0(qemuMonitorRdmaGidStatus, 1);

    info->netdev = g_strdup(netdev);

    info->gid_status = gid_status;
    info->subnet_prefix = subnet_prefix;
    info->interface_id = interface_id;

    processEvent = g_new0(struct qemuProcessEvent, 1);

    processEvent->eventType = QEMU_PROCESS_EVENT_RDMA_GID_STATUS_CHANGED;
    processEvent->vm = virObjectRef(vm);
    processEvent->data = g_steal_pointer(&info);

    if (virThreadPoolSendJob(driver->workerPool, 0, processEvent) < 0) {
        qemuProcessEventFree(processEvent);
        virObjectUnref(vm);
        goto cleanup;
    }

    ret = 0;
 cleanup:
    qemuMonitorEventRdmaGidStatusFree(info);
    virObjectUnlock(vm);
    return ret;
}