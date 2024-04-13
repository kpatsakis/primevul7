qemuProcessRefreshBalloonState(virQEMUDriverPtr driver,
                               virDomainObjPtr vm,
                               int asyncJob)
{
    unsigned long long balloon;
    int rc;

    /* if no ballooning is available, the current size equals to the current
     * full memory size */
    if (!virDomainDefHasMemballoon(vm->def)) {
        vm->def->mem.cur_balloon = virDomainDefGetMemoryTotal(vm->def);
        return 0;
    }

    if (qemuDomainObjEnterMonitorAsync(driver, vm, asyncJob) < 0)
        return -1;

    rc = qemuMonitorGetBalloonInfo(qemuDomainGetMonitor(vm), &balloon);
    if (qemuDomainObjExitMonitor(driver, vm) < 0 || rc < 0)
        return -1;

    vm->def->mem.cur_balloon = balloon;

    return 0;
}