qemuProcessHandleBlockThreshold(qemuMonitorPtr mon G_GNUC_UNUSED,
                                virDomainObjPtr vm,
                                const char *nodename,
                                unsigned long long threshold,
                                unsigned long long excess,
                                void *opaque)
{
    qemuDomainObjPrivatePtr priv;
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr eventSource = NULL;
    virObjectEventPtr eventDevice = NULL;
    virDomainDiskDefPtr disk;
    virStorageSourcePtr src;
    const char *path = NULL;

    virObjectLock(vm);

    priv  = vm->privateData;

    VIR_DEBUG("BLOCK_WRITE_THRESHOLD event for block node '%s' in domain %p %s:"
              "threshold '%llu' exceeded by '%llu'",
              nodename, vm, vm->def->name, threshold, excess);

    if ((disk = qemuDomainDiskLookupByNodename(vm->def, priv->backup, nodename, &src))) {
        if (virStorageSourceIsLocalStorage(src))
            path = src->path;

        if (src == disk->src) {
            g_autofree char *dev = qemuDomainDiskBackingStoreGetName(disk, 0);

            eventDevice = virDomainEventBlockThresholdNewFromObj(vm, dev, path,
                                                                 threshold, excess);
        }

        if (src->id != 0) {
            g_autofree char *dev = qemuDomainDiskBackingStoreGetName(disk, src->id);

            eventSource = virDomainEventBlockThresholdNewFromObj(vm, dev, path,
                                                                 threshold, excess);
        }
    }

    virObjectUnlock(vm);
    virObjectEventStateQueue(driver->domainEventState, eventDevice);
    virObjectEventStateQueue(driver->domainEventState, eventSource);

    return 0;
}