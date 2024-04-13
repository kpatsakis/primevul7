qemuProcessHandleTrayChange(qemuMonitorPtr mon G_GNUC_UNUSED,
                            virDomainObjPtr vm,
                            const char *devAlias,
                            const char *devid,
                            int reason,
                            void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr event = NULL;
    virDomainDiskDefPtr disk;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);

    virObjectLock(vm);
    disk = qemuProcessFindDomainDiskByAliasOrQOM(vm, devAlias, devid);

    if (disk) {
        event = virDomainEventTrayChangeNewFromObj(vm, disk->info.alias, reason);
        /* Update disk tray status */
        if (reason == VIR_DOMAIN_EVENT_TRAY_CHANGE_OPEN)
            disk->tray_status = VIR_DOMAIN_DISK_TRAY_OPEN;
        else if (reason == VIR_DOMAIN_EVENT_TRAY_CHANGE_CLOSE)
            disk->tray_status = VIR_DOMAIN_DISK_TRAY_CLOSED;

        if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0) {
            VIR_WARN("Unable to save status on vm %s after tray moved event",
                     vm->def->name);
        }

        virDomainObjBroadcast(vm);
    }

    virObjectUnlock(vm);
    virObjectEventStateQueue(driver->domainEventState, event);
    return 0;
}