qemuProcessRefreshDisks(virQEMUDriverPtr driver,
                        virDomainObjPtr vm,
                        qemuDomainAsyncJob asyncJob)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    bool blockdev = virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_BLOCKDEV);
    GHashTable *table = NULL;
    int ret = -1;
    size_t i;

    if (qemuDomainObjEnterMonitorAsync(driver, vm, asyncJob) == 0) {
        table = qemuMonitorGetBlockInfo(priv->mon);
        if (qemuDomainObjExitMonitor(driver, vm) < 0)
            goto cleanup;
    }

    if (!table)
        goto cleanup;

    for (i = 0; i < vm->def->ndisks; i++) {
        virDomainDiskDefPtr disk = vm->def->disks[i];
        qemuDomainDiskPrivatePtr diskpriv = QEMU_DOMAIN_DISK_PRIVATE(disk);
        struct qemuDomainDiskInfo *info;
        const char *entryname = disk->info.alias;

        if (blockdev && diskpriv->qomName)
            entryname = diskpriv->qomName;

        if (!(info = virHashLookup(table, entryname)))
            continue;

        if (info->removable) {
            if (info->empty)
                virDomainDiskEmptySource(disk);

            if (info->tray) {
                if (info->tray_open)
                    disk->tray_status = VIR_DOMAIN_DISK_TRAY_OPEN;
                else
                    disk->tray_status = VIR_DOMAIN_DISK_TRAY_CLOSED;
            }
        }

        /* fill in additional data */
        diskpriv->removable = info->removable;
        diskpriv->tray = info->tray;
    }

    ret = 0;

 cleanup:
    virHashFree(table);
    return ret;
}