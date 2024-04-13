qemuProcessFetchGuestCPU(virQEMUDriverPtr driver,
                         virDomainObjPtr vm,
                         qemuDomainAsyncJob asyncJob,
                         virCPUDataPtr *enabled,
                         virCPUDataPtr *disabled)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    g_autoptr(virCPUData) dataEnabled = NULL;
    g_autoptr(virCPUData) dataDisabled = NULL;
    bool generic;
    int rc;

    *enabled = NULL;
    *disabled = NULL;

    generic = virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_CPU_UNAVAILABLE_FEATURES);

    if (!generic && !ARCH_IS_X86(vm->def->os.arch))
        return 0;

    if (qemuDomainObjEnterMonitorAsync(driver, vm, asyncJob) < 0)
        return -1;

    if (generic) {
        rc = qemuMonitorGetGuestCPU(priv->mon,
                                    vm->def->os.arch,
                                    qemuProcessTranslateCPUFeatures, priv->qemuCaps,
                                    &dataEnabled, &dataDisabled);
    } else {
        rc = qemuMonitorGetGuestCPUx86(priv->mon, &dataEnabled, &dataDisabled);
    }

    if (qemuDomainObjExitMonitor(driver, vm) < 0)
        return -1;

    if (rc == -1)
        return -1;

    *enabled = g_steal_pointer(&dataEnabled);
    *disabled = g_steal_pointer(&dataDisabled);
    return 0;
}