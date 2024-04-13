qemuProcessKill(virDomainObjPtr vm, unsigned int flags)
{
    VIR_DEBUG("vm=%p name=%s pid=%lld flags=0x%x",
              vm, vm->def->name,
              (long long)vm->pid, flags);

    if (!(flags & VIR_QEMU_PROCESS_KILL_NOCHECK)) {
        if (!virDomainObjIsActive(vm)) {
            VIR_DEBUG("VM '%s' not active", vm->def->name);
            return 0;
        }
    }

    if (flags & VIR_QEMU_PROCESS_KILL_NOWAIT) {
        virProcessKill(vm->pid,
                       (flags & VIR_QEMU_PROCESS_KILL_FORCE) ?
                       SIGKILL : SIGTERM);
        return 0;
    }

    /* Request an extra delay of two seconds per current nhostdevs
     * to be safe against stalls by the kernel freeing up the resources */
    return virProcessKillPainfullyDelay(vm->pid,
                                        !!(flags & VIR_QEMU_PROCESS_KILL_FORCE),
                                        vm->def->nhostdevs * 2);
}