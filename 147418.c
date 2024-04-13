qemuProcessCreatePretendCmdPrepare(virQEMUDriverPtr driver,
                                   virDomainObjPtr vm,
                                   const char *migrateURI,
                                   bool standalone,
                                   unsigned int flags)
{
    virCheckFlags(VIR_QEMU_PROCESS_START_COLD |
                  VIR_QEMU_PROCESS_START_PAUSED |
                  VIR_QEMU_PROCESS_START_AUTODESTROY, -1);

    flags |= VIR_QEMU_PROCESS_START_PRETEND;

    if (!migrateURI)
        flags |= VIR_QEMU_PROCESS_START_NEW;

    if (standalone)
        flags |= VIR_QEMU_PROCESS_START_STANDALONE;

    if (qemuProcessInit(driver, vm, NULL, QEMU_ASYNC_JOB_NONE,
                        !!migrateURI, flags) < 0)
        return -1;

    if (qemuProcessPrepareDomain(driver, vm, flags) < 0)
        return -1;

    return 0;
}