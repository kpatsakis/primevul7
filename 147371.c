qemuProcessSetupVcpu(virDomainObjPtr vm,
                     unsigned int vcpuid)
{
    pid_t vcpupid = qemuDomainGetVcpuPid(vm, vcpuid);
    virDomainVcpuDefPtr vcpu = virDomainDefGetVcpu(vm->def, vcpuid);
    virDomainResctrlMonDefPtr mon = NULL;
    size_t i = 0;

    if (qemuProcessSetupPid(vm, vcpupid, VIR_CGROUP_THREAD_VCPU,
                            vcpuid, vcpu->cpumask,
                            vm->def->cputune.period,
                            vm->def->cputune.quota,
                            &vcpu->sched) < 0)
        return -1;

    for (i = 0; i < vm->def->nresctrls; i++) {
        size_t j = 0;
        virDomainResctrlDefPtr ct = vm->def->resctrls[i];

        if (virBitmapIsBitSet(ct->vcpus, vcpuid)) {
            if (virResctrlAllocAddPID(ct->alloc, vcpupid) < 0)
                return -1;

            for (j = 0; j < ct->nmonitors; j++) {
                mon = ct->monitors[j];

                if (virBitmapEqual(ct->vcpus, mon->vcpus) &&
                    !virResctrlAllocIsEmpty(ct->alloc))
                    continue;

                if (virBitmapIsBitSet(mon->vcpus, vcpuid)) {
                    if (virResctrlMonitorAddPID(mon->instance, vcpupid) < 0)
                        return -1;
                    break;
                }
            }

            break;
        }
    }

    return 0;
}