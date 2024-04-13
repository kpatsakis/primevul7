qemuRefreshPRManagerState(virQEMUDriverPtr driver,
                          virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    GHashTable *info = NULL;
    int ret = -1;

    if (!virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_PR_MANAGER_HELPER) ||
        !qemuDomainDefHasManagedPR(vm))
        return 0;

    qemuDomainObjEnterMonitor(driver, vm);
    ret = qemuMonitorGetPRManagerInfo(priv->mon, &info);
    if (qemuDomainObjExitMonitor(driver, vm) < 0)
        ret = -1;

    if (ret < 0)
        goto cleanup;

    ret = qemuProcessRefreshPRManagerState(vm, info);

 cleanup:
    virHashFree(info);
    return ret;
}