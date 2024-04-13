qemuProcessUpdateAndVerifyCPU(virQEMUDriverPtr driver,
                              virDomainObjPtr vm,
                              qemuDomainAsyncJob asyncJob)
{
    virCPUDataPtr cpu = NULL;
    virCPUDataPtr disabled = NULL;
    int ret = -1;

    if (qemuProcessFetchGuestCPU(driver, vm, asyncJob, &cpu, &disabled) < 0)
        goto cleanup;

    if (qemuProcessVerifyCPU(vm, cpu) < 0)
        goto cleanup;

    if (qemuProcessUpdateLiveGuestCPU(vm, cpu, disabled) < 0)
        goto cleanup;

    ret = 0;

 cleanup:
    virCPUDataFree(cpu);
    virCPUDataFree(disabled);
    return ret;
}