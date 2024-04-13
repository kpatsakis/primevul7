qemuProcessHandleIOError(qemuMonitorPtr mon G_GNUC_UNUSED,
                         virDomainObjPtr vm,
                         const char *diskAlias,
                         const char *nodename,
                         int action,
                         const char *reason,
                         void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr ioErrorEvent = NULL;
    virObjectEventPtr ioErrorEvent2 = NULL;
    virObjectEventPtr lifecycleEvent = NULL;
    const char *srcPath;
    const char *devAlias;
    virDomainDiskDefPtr disk;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);

    virObjectLock(vm);

    if (*diskAlias == '\0')
        diskAlias = NULL;

    if (diskAlias)
        disk = qemuProcessFindDomainDiskByAliasOrQOM(vm, diskAlias, NULL);
    else if (nodename)
        disk = qemuDomainDiskLookupByNodename(vm->def, NULL, nodename, NULL);
    else
        disk = NULL;

    if (disk) {
        srcPath = virDomainDiskGetSource(disk);
        devAlias = disk->info.alias;
    } else {
        srcPath = "";
        devAlias = "";
    }

    ioErrorEvent = virDomainEventIOErrorNewFromObj(vm, srcPath, devAlias, action);
    ioErrorEvent2 = virDomainEventIOErrorReasonNewFromObj(vm, srcPath, devAlias, action, reason);

    if (action == VIR_DOMAIN_EVENT_IO_ERROR_PAUSE &&
        virDomainObjGetState(vm, NULL) == VIR_DOMAIN_RUNNING) {
        qemuDomainObjPrivatePtr priv = vm->privateData;
        VIR_DEBUG("Transitioned guest %s to paused state due to IO error", vm->def->name);

        if (priv->signalIOError)
            virDomainObjBroadcast(vm);

        virDomainObjSetState(vm, VIR_DOMAIN_PAUSED, VIR_DOMAIN_PAUSED_IOERROR);
        lifecycleEvent = virDomainEventLifecycleNewFromObj(vm,
                                                  VIR_DOMAIN_EVENT_SUSPENDED,
                                                  VIR_DOMAIN_EVENT_SUSPENDED_IOERROR);

        VIR_FREE(priv->lockState);
        if (virDomainLockProcessPause(driver->lockManager, vm, &priv->lockState) < 0)
            VIR_WARN("Unable to release lease on %s", vm->def->name);
        VIR_DEBUG("Preserving lock state '%s'", NULLSTR(priv->lockState));

        if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0)
            VIR_WARN("Unable to save status on vm %s after IO error", vm->def->name);
    }
    virObjectUnlock(vm);

    virObjectEventStateQueue(driver->domainEventState, ioErrorEvent);
    virObjectEventStateQueue(driver->domainEventState, ioErrorEvent2);
    virObjectEventStateQueue(driver->domainEventState, lifecycleEvent);
    return 0;
}