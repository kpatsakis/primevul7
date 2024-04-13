qemuProcessUpdateGuestCPU(virDomainDefPtr def,
                          virQEMUCapsPtr qemuCaps,
                          virArch hostarch,
                          unsigned int flags)
{
    if (!def->cpu)
        return 0;

    /* nothing to do if only topology part of CPU def is used */
    if (def->cpu->mode == VIR_CPU_MODE_CUSTOM && !def->cpu->model)
        return 0;

    /* Old libvirt added host CPU model to host-model CPUs for migrations,
     * while new libvirt just turns host-model into custom mode. We need
     * to fix the mode to maintain backward compatibility and to avoid
     * the CPU model to be replaced in virCPUUpdate.
     */
    if (!(flags & VIR_QEMU_PROCESS_START_NEW) &&
        ARCH_IS_X86(def->os.arch) &&
        def->cpu->mode == VIR_CPU_MODE_HOST_MODEL &&
        def->cpu->model) {
        def->cpu->mode = VIR_CPU_MODE_CUSTOM;
    }

    if (!virQEMUCapsIsCPUModeSupported(qemuCaps, hostarch, def->virtType,
                                       def->cpu->mode, def->os.machine)) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                       _("CPU mode '%s' for %s %s domain on %s host is not "
                         "supported by hypervisor"),
                       virCPUModeTypeToString(def->cpu->mode),
                       virArchToString(def->os.arch),
                       virDomainVirtTypeToString(def->virtType),
                       virArchToString(hostarch));
        return -1;
    }

    if (virCPUConvertLegacy(hostarch, def->cpu) < 0)
        return -1;

    /* nothing to update for host-passthrough / maximum */
    if (def->cpu->mode != VIR_CPU_MODE_HOST_PASSTHROUGH &&
        def->cpu->mode != VIR_CPU_MODE_MAXIMUM) {
        g_autoptr(virDomainCapsCPUModels) cpuModels = NULL;

        if (def->cpu->check == VIR_CPU_CHECK_PARTIAL &&
            virCPUCompare(hostarch,
                          virQEMUCapsGetHostModel(qemuCaps, def->virtType,
                                                  VIR_QEMU_CAPS_HOST_CPU_FULL),
                          def->cpu, true) < 0)
            return -1;

        if (virCPUUpdate(def->os.arch, def->cpu,
                         virQEMUCapsGetHostModel(qemuCaps, def->virtType,
                                                 VIR_QEMU_CAPS_HOST_CPU_MIGRATABLE)) < 0)
            return -1;

        cpuModels = virQEMUCapsGetCPUModels(qemuCaps, def->virtType, NULL, NULL);

        if (virCPUTranslate(def->os.arch, def->cpu, cpuModels) < 0)
            return -1;

        def->cpu->fallback = VIR_CPU_FALLBACK_FORBID;
    }

    if (virCPUDefFilterFeatures(def->cpu, virQEMUCapsCPUFilterFeatures,
                                &def->os.arch) < 0)
        return -1;

    if (ARCH_IS_X86(def->os.arch)) {
        g_auto(GStrv) features = NULL;

        if (virQEMUCapsGetCPUFeatures(qemuCaps, def->virtType, false, &features) < 0)
            return -1;

        if (features &&
            virCPUDefFilterFeatures(def->cpu, qemuProcessDropUnknownCPUFeatures,
                                    features) < 0)
            return -1;
    }

    return 0;
}