qemuProcessPrepareHost(virQEMUDriverPtr driver,
                       virDomainObjPtr vm,
                       unsigned int flags)
{
    unsigned int hostdev_flags = 0;
    qemuDomainObjPrivatePtr priv = vm->privateData;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);

    if (qemuPrepareNVRAM(cfg, vm) < 0)
        return -1;

    if (vm->def->vsock) {
        if (qemuProcessOpenVhostVsock(vm->def->vsock) < 0)
            return -1;
    }
    /* network devices must be "prepared" before hostdevs, because
     * setting up a network device might create a new hostdev that
     * will need to be setup.
     */
    VIR_DEBUG("Preparing network devices");
    if (qemuProcessNetworkPrepareDevices(driver, vm) < 0)
        return -1;

    /* Must be run before security labelling */
    VIR_DEBUG("Preparing host devices");
    if (!cfg->relaxedACS)
        hostdev_flags |= VIR_HOSTDEV_STRICT_ACS_CHECK;
    if (flags & VIR_QEMU_PROCESS_START_NEW)
        hostdev_flags |= VIR_HOSTDEV_COLD_BOOT;
    if (qemuHostdevPrepareDomainDevices(driver, vm->def, priv->qemuCaps,
                                        hostdev_flags) < 0)
        return -1;

    VIR_DEBUG("Preparing chr devices");
    if (virDomainChrDefForeach(vm->def,
                               true,
                               qemuProcessPrepareChardevDevice,
                               NULL) < 0)
        return -1;

    if (qemuProcessBuildDestroyMemoryPaths(driver, vm, NULL, true) < 0)
        return -1;

    /* Ensure no historical cgroup for this VM is lying around bogus
     * settings */
    VIR_DEBUG("Ensuring no historical cgroup is lying around");
    qemuRemoveCgroup(vm);

    if (virFileMakePath(cfg->logDir) < 0) {
        virReportSystemError(errno,
                             _("cannot create log directory %s"),
                             cfg->logDir);
        return -1;
    }

    VIR_FREE(priv->pidfile);
    if (!(priv->pidfile = virPidFileBuildPath(cfg->stateDir, vm->def->name))) {
        virReportSystemError(errno,
                             "%s", _("Failed to build pidfile path."));
        return -1;
    }

    if (unlink(priv->pidfile) < 0 &&
        errno != ENOENT) {
        virReportSystemError(errno,
                             _("Cannot remove stale PID file %s"),
                             priv->pidfile);
        return -1;
    }

    /*
     * Create all per-domain directories in order to make sure domain
     * with any possible seclabels can access it.
     */
    if (qemuProcessMakeDir(driver, vm, priv->libDir) < 0 ||
        qemuProcessMakeDir(driver, vm, priv->channelTargetDir) < 0)
        return -1;

    VIR_DEBUG("Write domain masterKey");
    if (qemuDomainWriteMasterKeyFile(driver, vm) < 0)
        return -1;

    VIR_DEBUG("Preparing disks (host)");
    if (qemuProcessPrepareHostStorage(driver, vm, flags) < 0)
        return -1;

    VIR_DEBUG("Preparing hostdevs (host-side)");
    if (qemuProcessPrepareHostHostdevs(vm) < 0)
        return -1;

    VIR_DEBUG("Preparing external devices");
    if (qemuExtDevicesPrepareHost(driver, vm) < 0)
        return -1;

    if (qemuProcessPrepareSEVGuestInput(vm) < 0)
        return -1;

    return 0;
}