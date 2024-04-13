qemuProcessPrepareHostStorage(virQEMUDriverPtr driver,
                              virDomainObjPtr vm,
                              unsigned int flags)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    size_t i;
    bool cold_boot = flags & VIR_QEMU_PROCESS_START_COLD;
    bool blockdev = virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_BLOCKDEV);

    for (i = vm->def->ndisks; i > 0; i--) {
        size_t idx = i - 1;
        virDomainDiskDefPtr disk = vm->def->disks[idx];

        if (virStorageSourceIsEmpty(disk->src))
            continue;

        /* backing chain needs to be redetected if we aren't using blockdev */
        if (!blockdev || qemuDiskBusIsSD(disk->bus))
            virStorageSourceBackingStoreClear(disk->src);

        /*
         * Go to applying startup policy for optional disk with nonexistent
         * source file immediately as determining chain will surely fail
         * and we don't want noisy error notice in logs for this case.
         */
        if (qemuDomainDiskIsMissingLocalOptional(disk) && cold_boot)
            VIR_INFO("optional disk '%s' source file is missing, "
                     "skip checking disk chain", disk->dst);
        else if (qemuDomainDetermineDiskChain(driver, vm, disk, NULL, true) >= 0)
            continue;

        if (qemuDomainCheckDiskStartupPolicy(driver, vm, idx, cold_boot) >= 0)
            continue;

        return -1;
    }

    return 0;
}