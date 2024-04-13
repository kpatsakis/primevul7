qemuProcessSetupDiskThrottlingBlockdev(virQEMUDriverPtr driver,
                                       virDomainObjPtr vm,
                                       qemuDomainAsyncJob asyncJob)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    size_t i;
    int ret = -1;

    if (!virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_BLOCKDEV))
        return 0;

    VIR_DEBUG("Setting up disk throttling for -blockdev via block_set_io_throttle");

    if (qemuDomainObjEnterMonitorAsync(driver, vm, asyncJob) < 0)
        return -1;

    for (i = 0; i < vm->def->ndisks; i++) {
        virDomainDiskDefPtr disk = vm->def->disks[i];
        qemuDomainDiskPrivatePtr diskPriv = QEMU_DOMAIN_DISK_PRIVATE(disk);

        /* sd-cards are instantiated via -drive */
        if (qemuDiskBusIsSD(disk->bus))
            continue;

        /* Setting throttling for empty drives fails */
        if (virStorageSourceIsEmpty(disk->src))
            continue;

        if (!qemuDiskConfigBlkdeviotuneEnabled(disk))
            continue;

        if (qemuMonitorSetBlockIoThrottle(qemuDomainGetMonitor(vm), NULL,
                                          diskPriv->qomName, &disk->blkdeviotune,
                                          true, true, true) < 0)
            goto cleanup;
    }

    ret = 0;

 cleanup:
    if (qemuDomainObjExitMonitor(driver, vm) < 0)
        ret = -1;
    return ret;
}