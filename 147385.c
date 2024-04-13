qemuProcessUpdateCPU(virQEMUDriverPtr driver,
                     virDomainObjPtr vm,
                     qemuDomainAsyncJob asyncJob)
{
    g_autoptr(virCPUData) cpu = NULL;
    g_autoptr(virCPUData) disabled = NULL;
    g_autoptr(virDomainCapsCPUModels) models = NULL;

    /* The host CPU model comes from host caps rather than QEMU caps so
     * fallback must be allowed no matter what the user specified in the XML.
     */
    vm->def->cpu->fallback = VIR_CPU_FALLBACK_ALLOW;

    if (qemuProcessFetchGuestCPU(driver, vm, asyncJob, &cpu, &disabled) < 0)
        return -1;

    if (qemuProcessUpdateLiveGuestCPU(vm, cpu, disabled) < 0)
        return -1;

    if (qemuProcessFetchCPUDefinitions(driver, vm, asyncJob, &models) < 0 ||
        virCPUTranslate(vm->def->os.arch, vm->def->cpu, models) < 0)
        return -1;

    return 0;
}