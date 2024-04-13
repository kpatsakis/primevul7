qemuProcessFetchCPUDefinitions(virQEMUDriverPtr driver,
                               virDomainObjPtr vm,
                               qemuDomainAsyncJob asyncJob,
                               virDomainCapsCPUModelsPtr *cpuModels)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    g_autoptr(virDomainCapsCPUModels) models = NULL;
    int rc;

    if (qemuDomainObjEnterMonitorAsync(driver, vm, asyncJob) < 0)
        return -1;

    rc = virQEMUCapsFetchCPUModels(priv->mon, vm->def->os.arch, &models);

    if (qemuDomainObjExitMonitor(driver, vm) < 0 || rc < 0)
        return -1;

    *cpuModels = g_steal_pointer(&models);
    return 0;
}