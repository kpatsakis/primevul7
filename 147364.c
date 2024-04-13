qemuProcessInitCpuAffinity(virDomainObjPtr vm)
{
    bool settingAll = false;
    g_autoptr(virBitmap) cpumapToSet = NULL;
    virDomainNumatuneMemMode mem_mode;
    qemuDomainObjPrivatePtr priv = vm->privateData;

    if (!vm->pid) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("Cannot setup CPU affinity until process is started"));
        return -1;
    }

    /* Here is the deal, we can't set cpuset.mems before qemu is
     * started as it clashes with KVM allocation. Therefore, we
     * used to let qemu allocate its memory anywhere as we would
     * then move the memory to desired NUMA node via CGroups.
     * However, that might not be always possible because qemu
     * might lock some parts of its memory (e.g. due to VFIO).
     * Even if it possible, memory has to be copied between NUMA
     * nodes which is suboptimal.
     * Solution is to set affinity that matches the best what we
     * would have set in CGroups and then fix it later, once qemu
     * is already running. */
    if (virDomainNumaGetNodeCount(vm->def->numa) <= 1 &&
        virDomainNumatuneGetMode(vm->def->numa, -1, &mem_mode) == 0 &&
        mem_mode == VIR_DOMAIN_NUMATUNE_MEM_STRICT) {
        virBitmapPtr nodeset = NULL;

        if (virDomainNumatuneMaybeGetNodeset(vm->def->numa,
                                             priv->autoNodeset,
                                             &nodeset,
                                             -1) < 0)
            return -1;

        if (virNumaNodesetToCPUset(nodeset, &cpumapToSet) < 0)
            return -1;
    } else if (vm->def->cputune.emulatorpin) {
        cpumapToSet = virBitmapNewCopy(vm->def->cputune.emulatorpin);
    } else {
        settingAll = true;
        if (qemuProcessGetAllCpuAffinity(&cpumapToSet) < 0)
            return -1;
    }

    /*
     * We only want to error out if we failed to set the affinity to
     * user-requested mapping.  If we are just trying to reset the affinity
     * to all CPUs and this fails it can only be an issue if:
     *  1) libvirtd does not have CAP_SYS_NICE
     *  2) libvirtd does not run on all CPUs
     *
     * This scenario can easily occur when libvirtd is run inside a
     * container with restrictive permissions and CPU pinning.
     *
     * See also: https://bugzilla.redhat.com/1819801#c2
     */
    if (cpumapToSet &&
        virProcessSetAffinity(vm->pid, cpumapToSet, settingAll) < 0) {
        return -1;
    }

    return 0;
}