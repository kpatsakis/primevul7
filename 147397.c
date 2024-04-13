qemuProcessStartValidate(virQEMUDriverPtr driver,
                         virDomainObjPtr vm,
                         virQEMUCapsPtr qemuCaps,
                         unsigned int flags)
{
    if (!(flags & VIR_QEMU_PROCESS_START_PRETEND)) {
        if (vm->def->virtType == VIR_DOMAIN_VIRT_KVM) {
            VIR_DEBUG("Checking for KVM availability");
            if (!virFileExists("/dev/kvm")) {
                virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                               _("Domain requires KVM, but it is not available. "
                                 "Check that virtualization is enabled in the "
                                 "host BIOS, and host configuration is setup to "
                                 "load the kvm modules."));
                return -1;
            }
        }

        VIR_DEBUG("Checking domain and device security labels");
        if (qemuSecurityCheckAllLabel(driver->securityManager, vm->def) < 0)
            return -1;

    }

    /* Checks below should not be executed when starting a qemu process for a
     * VM that was running before (migration, snapshots, save). It's more
     * important to start such VM than keep the configuration clean */
    if ((flags & VIR_QEMU_PROCESS_START_NEW) &&
        virDomainDefValidate(vm->def, 0, driver->xmlopt, qemuCaps) < 0)
        return -1;

    if (qemuProcessStartValidateGraphics(vm) < 0)
        return -1;

    if (qemuProcessStartValidateIOThreads(vm, qemuCaps) < 0)
        return -1;

    if (qemuProcessStartValidateShmem(vm) < 0)
        return -1;

    if (vm->def->cpu) {
        if (virCPUValidateFeatures(vm->def->os.arch, vm->def->cpu) < 0)
            return -1;

        if (ARCH_IS_X86(vm->def->os.arch) &&
            !virQEMUCapsGet(qemuCaps, QEMU_CAPS_CPU_UNAVAILABLE_FEATURES)) {
            g_auto(GStrv) features = NULL;
            int n;

            if ((n = virCPUDefCheckFeatures(vm->def->cpu,
                                            virCPUx86FeatureFilterSelectMSR,
                                            NULL,
                                            &features)) < 0)
                return -1;

            if (n > 0) {
                g_autofree char *str = NULL;

                str = g_strjoinv(", ", features);
                virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                               _("Some features cannot be reliably used "
                                 "with this QEMU: %s"), str);
                return -1;
            }
        }
    }

    if (qemuProcessStartValidateDisks(vm, qemuCaps) < 0)
        return -1;

    if (qemuProcessStartValidateTSC(driver, vm) < 0)
        return -1;

    VIR_DEBUG("Checking for any possible (non-fatal) issues");

    qemuProcessStartWarnShmem(vm);

    return 0;
}