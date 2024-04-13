qemuProcessPrepareDomainStorage(virQEMUDriverPtr driver,
                                virDomainObjPtr vm,
                                qemuDomainObjPrivatePtr priv,
                                virQEMUDriverConfigPtr cfg,
                                unsigned int flags)
{
    size_t i;
    bool cold_boot = flags & VIR_QEMU_PROCESS_START_COLD;

    for (i = vm->def->ndisks; i > 0; i--) {
        size_t idx = i - 1;
        virDomainDiskDefPtr disk = vm->def->disks[idx];

        if (virDomainDiskTranslateSourcePool(disk) < 0) {
            if (qemuDomainCheckDiskStartupPolicy(driver, vm, idx, cold_boot) < 0)
                return -1;

            /* disk source was dropped */
            continue;
        }

        if (qemuDomainPrepareDiskSource(disk, priv, cfg) < 0)
            return -1;
    }

    return 0;
}