qemuProcessSetupEmulator(virDomainObjPtr vm)
{
    return qemuProcessSetupPid(vm, vm->pid, VIR_CGROUP_THREAD_EMULATOR,
                               0, vm->def->cputune.emulatorpin,
                               vm->def->cputune.emulator_period,
                               vm->def->cputune.emulator_quota,
                               vm->def->cputune.emulatorsched);
}