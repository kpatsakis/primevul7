qemuProcessRefreshLegacyBlockjobs(virQEMUDriverPtr driver,
                                  virDomainObjPtr vm)
{
    GHashTable *blockJobs = NULL;
    int ret = -1;

    qemuDomainObjEnterMonitor(driver, vm);
    blockJobs = qemuMonitorGetAllBlockJobInfo(qemuDomainGetMonitor(vm), true);
    if (qemuDomainObjExitMonitor(driver, vm) < 0 || !blockJobs)
        goto cleanup;

    if (virHashForEach(blockJobs, qemuProcessRefreshLegacyBlockjob, vm) < 0)
        goto cleanup;

    ret = 0;

 cleanup:
    virHashFree(blockJobs);
    return ret;
}