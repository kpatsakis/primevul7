qemuProcessVerifyCPU(virDomainObjPtr vm,
                     virCPUDataPtr cpu)
{
    virDomainDefPtr def = vm->def;

    if (!cpu)
        return 0;

    if (qemuProcessVerifyKVMFeatures(def, cpu) < 0 ||
        qemuProcessVerifyHypervFeatures(def, cpu) < 0)
        return -1;

    if (!def->cpu ||
        (def->cpu->mode == VIR_CPU_MODE_CUSTOM &&
         !def->cpu->model))
        return 0;

    if (qemuProcessVerifyCPUFeatures(def, cpu) < 0)
        return -1;

    return 0;
}