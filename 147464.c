qemuProcessBeginJob(virQEMUDriverPtr driver,
                    virDomainObjPtr vm,
                    virDomainJobOperation operation,
                    unsigned long apiFlags)
{
    if (qemuDomainObjBeginAsyncJob(driver, vm, QEMU_ASYNC_JOB_START,
                                   operation, apiFlags) < 0)
        return -1;

    qemuDomainObjSetAsyncJobMask(vm, QEMU_JOB_NONE);
    return 0;
}