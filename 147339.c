qemuProcessRefreshCPU(virQEMUDriverPtr driver,
                      virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    g_autoptr(virCPUDef) host = NULL;
    g_autoptr(virCPUDef) hostmig = NULL;
    g_autoptr(virCPUDef) cpu = NULL;

    if (!virQEMUCapsGuestIsNative(driver->hostarch, vm->def->os.arch))
        return 0;

    if (!vm->def->cpu)
        return 0;

    if (qemuProcessRefreshCPUMigratability(driver, vm, QEMU_ASYNC_JOB_NONE) < 0)
        return -1;

    if (!(host = virQEMUDriverGetHostCPU(driver))) {
        virResetLastError();
        return 0;
    }

    /* If the domain with a host-model CPU was started by an old libvirt
     * (< 2.3) which didn't replace the CPU with a custom one, let's do it now
     * since the rest of our code does not really expect a host-model CPU in a
     * running domain.
     */
    if (vm->def->cpu->mode == VIR_CPU_MODE_HOST_MODEL) {
        /*
         * PSeries domains are able to run with host-model CPU by design,
         * even on Libvirt newer than 2.3, never replacing host-model with
         * custom in the virCPUUpdate() call. It is not needed to call
         * virCPUUpdate() and qemuProcessUpdateCPU() in this case.
         */
        if (qemuDomainIsPSeries(vm->def))
            return 0;

        if (!(hostmig = virCPUCopyMigratable(host->arch, host)))
            return -1;

        if (!(cpu = virCPUDefCopyWithoutModel(hostmig)) ||
            virCPUDefCopyModelFilter(cpu, hostmig, false,
                                     virQEMUCapsCPUFilterFeatures,
                                     &host->arch) < 0)
            return -1;

        if (virCPUUpdate(vm->def->os.arch, vm->def->cpu, cpu) < 0)
            return -1;

        if (qemuProcessUpdateCPU(driver, vm, QEMU_ASYNC_JOB_NONE) < 0)
            return -1;
    } else if (!virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_QUERY_CPU_MODEL_EXPANSION)) {
        /* We only try to fix CPUs when the libvirt/QEMU combo used to start
         * the domain did not know about query-cpu-model-expansion in which
         * case the host-model is known to not contain features which QEMU
         * doesn't know about.
         */
        if (qemuDomainFixupCPUs(vm, &priv->origCPU) < 0)
            return -1;
    }

    return 0;
}