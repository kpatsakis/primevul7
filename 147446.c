qemuProcessEndJob(virQEMUDriverPtr driver,
                  virDomainObjPtr vm)
{
    qemuDomainObjEndAsyncJob(driver, vm);
}