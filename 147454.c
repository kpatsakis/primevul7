qemuProcessRefreshCPUMigratability(virQEMUDriverPtr driver,
                                   virDomainObjPtr vm,
                                   qemuDomainAsyncJob asyncJob)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    virDomainDefPtr def = vm->def;
    bool migratable;
    int rc;

    if (def->cpu->mode != VIR_CPU_MODE_HOST_PASSTHROUGH &&
        def->cpu->mode != VIR_CPU_MODE_MAXIMUM)
        return 0;

    /* If the cpu.migratable capability is present, the migratable attribute
     * is set correctly. */
    if (virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_CPU_MIGRATABLE))
        return 0;

    if (!ARCH_IS_X86(def->os.arch))
        return 0;

    if (qemuDomainObjEnterMonitorAsync(driver, vm, asyncJob) < 0)
        return -1;

    rc = qemuMonitorGetCPUMigratable(priv->mon, &migratable);

    if (qemuDomainObjExitMonitor(driver, vm) < 0 || rc < 0)
        return -1;

    if (rc == 1)
        migratable = false;

    /* Libvirt 6.5.0 would set migratable='off' for running domains even though
     * the actual default used by QEMU was 'on'. */
    if (def->cpu->migratable == VIR_TRISTATE_SWITCH_OFF && migratable) {
        VIR_DEBUG("Fixing CPU migratable attribute");
        def->cpu->migratable = VIR_TRISTATE_SWITCH_ON;
    }

    if (def->cpu->migratable == VIR_TRISTATE_SWITCH_ABSENT)
        def->cpu->migratable = virTristateSwitchFromBool(migratable);

    return 0;
}