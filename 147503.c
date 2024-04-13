qemuProcessLaunch(virConnectPtr conn,
                  virQEMUDriverPtr driver,
                  virDomainObjPtr vm,
                  qemuDomainAsyncJob asyncJob,
                  qemuProcessIncomingDefPtr incoming,
                  virDomainMomentObjPtr snapshot,
                  virNetDevVPortProfileOp vmop,
                  unsigned int flags)
{
    int ret = -1;
    int rv;
    int logfile = -1;
    g_autoptr(qemuDomainLogContext) logCtxt = NULL;
    qemuDomainObjPrivatePtr priv = vm->privateData;
    g_autoptr(virCommand) cmd = NULL;
    struct qemuProcessHookData hookData;
    g_autoptr(virQEMUDriverConfig) cfg = NULL;
    size_t nnicindexes = 0;
    g_autofree int *nicindexes = NULL;

    VIR_DEBUG("conn=%p driver=%p vm=%p name=%s if=%d asyncJob=%d "
              "incoming.launchURI=%s incoming.deferredURI=%s "
              "incoming.fd=%d incoming.path=%s "
              "snapshot=%p vmop=%d flags=0x%x",
              conn, driver, vm, vm->def->name, vm->def->id, asyncJob,
              NULLSTR(incoming ? incoming->launchURI : NULL),
              NULLSTR(incoming ? incoming->deferredURI : NULL),
              incoming ? incoming->fd : -1,
              NULLSTR(incoming ? incoming->path : NULL),
              snapshot, vmop, flags);

    /* Okay, these are just internal flags,
     * but doesn't hurt to check */
    virCheckFlags(VIR_QEMU_PROCESS_START_COLD |
                  VIR_QEMU_PROCESS_START_PAUSED |
                  VIR_QEMU_PROCESS_START_AUTODESTROY |
                  VIR_QEMU_PROCESS_START_NEW |
                  VIR_QEMU_PROCESS_START_GEN_VMID, -1);

    cfg = virQEMUDriverGetConfig(driver);

    if (flags & VIR_QEMU_PROCESS_START_AUTODESTROY) {
        if (!conn) {
            virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                           _("Domain autodestroy requires a connection handle"));
            return -1;
        }
        if (driver->embeddedRoot) {
            virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                           _("Domain autodestroy not supported for embedded drivers yet"));
            return -1;
        }
    }

    hookData.vm = vm;
    hookData.driver = driver;
    /* We don't increase cfg's reference counter here. */
    hookData.cfg = cfg;

    VIR_DEBUG("Creating domain log file");
    if (!(logCtxt = qemuDomainLogContextNew(driver, vm,
                                            QEMU_DOMAIN_LOG_CONTEXT_MODE_START))) {
        virLastErrorPrefixMessage("%s", _("can't connect to virtlogd"));
        goto cleanup;
    }
    logfile = qemuDomainLogContextGetWriteFD(logCtxt);

    if (qemuProcessGenID(vm, flags) < 0)
        goto cleanup;

    if (qemuExtDevicesStart(driver, vm,
                            qemuDomainLogContextGetManager(logCtxt),
                            incoming != NULL) < 0)
        goto cleanup;

    VIR_DEBUG("Building emulator command line");
    if (!(cmd = qemuBuildCommandLine(driver,
                                     qemuDomainLogContextGetManager(logCtxt),
                                     driver->securityManager,
                                     vm,
                                     incoming ? incoming->launchURI : NULL,
                                     snapshot, vmop,
                                     false,
                                     qemuCheckFips(vm),
                                     &nnicindexes, &nicindexes, 0)))
        goto cleanup;

    if (incoming && incoming->fd != -1)
        virCommandPassFD(cmd, incoming->fd, 0);

    /* now that we know it is about to start call the hook if present */
    if (qemuProcessStartHook(driver, vm,
                             VIR_HOOK_QEMU_OP_START,
                             VIR_HOOK_SUBOP_BEGIN) < 0)
        goto cleanup;

    qemuLogOperation(vm, "starting up", cmd, logCtxt);

    qemuDomainObjCheckTaint(driver, vm, logCtxt, incoming != NULL);

    qemuDomainLogContextMarkPosition(logCtxt);

    VIR_DEBUG("Building mount namespace");

    if (qemuProcessEnableDomainNamespaces(driver, vm) < 0)
        goto cleanup;

    VIR_DEBUG("Setting up raw IO");
    if (qemuProcessSetupRawIO(driver, vm, cmd) < 0)
        goto cleanup;

    virCommandSetPreExecHook(cmd, qemuProcessHook, &hookData);
    virCommandSetMaxProcesses(cmd, cfg->maxProcesses);
    virCommandSetMaxFiles(cmd, cfg->maxFiles);
    virCommandSetMaxCoreSize(cmd, cfg->maxCore);
    virCommandSetUmask(cmd, 0x002);

    VIR_DEBUG("Setting up security labelling");
    if (qemuSecuritySetChildProcessLabel(driver->securityManager,
                                         vm->def, cmd) < 0)
        goto cleanup;

    virCommandSetOutputFD(cmd, &logfile);
    virCommandSetErrorFD(cmd, &logfile);
    virCommandNonblockingFDs(cmd);
    virCommandSetPidFile(cmd, priv->pidfile);
    virCommandDaemonize(cmd);
    virCommandRequireHandshake(cmd);

    if (qemuSecurityPreFork(driver->securityManager) < 0)
        goto cleanup;
    rv = virCommandRun(cmd, NULL);
    qemuSecurityPostFork(driver->securityManager);

    /* wait for qemu process to show up */
    if (rv == 0) {
        if ((rv = virPidFileReadPath(priv->pidfile, &vm->pid)) < 0) {
            virReportSystemError(-rv,
                                 _("Domain %s didn't show up"),
                                 vm->def->name);
            goto cleanup;
        }
        VIR_DEBUG("QEMU vm=%p name=%s running with pid=%lld",
                  vm, vm->def->name, (long long)vm->pid);
    } else {
        VIR_DEBUG("QEMU vm=%p name=%s failed to spawn",
                  vm, vm->def->name);
        goto cleanup;
    }

    VIR_DEBUG("Writing early domain status to disk");
    if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0)
        goto cleanup;

    VIR_DEBUG("Waiting for handshake from child");
    if (virCommandHandshakeWait(cmd) < 0) {
        /* Read errors from child that occurred between fork and exec. */
        qemuProcessReportLogError(logCtxt,
                                  _("Process exited prior to exec"));
        goto cleanup;
    }

    VIR_DEBUG("Building domain mount namespace (if required)");
    if (qemuDomainBuildNamespace(cfg, vm) < 0)
        goto cleanup;

    VIR_DEBUG("Setting up domain cgroup (if required)");
    if (qemuSetupCgroup(vm, nnicindexes, nicindexes) < 0)
        goto cleanup;

    VIR_DEBUG("Setting up domain perf (if required)");
    if (qemuProcessEnablePerf(vm) < 0)
        goto cleanup;

    /* This must be done after cgroup placement to avoid resetting CPU
     * affinity */
    if (qemuProcessInitCpuAffinity(vm) < 0)
        goto cleanup;

    VIR_DEBUG("Setting emulator tuning/settings");
    if (qemuProcessSetupEmulator(vm) < 0)
        goto cleanup;

    VIR_DEBUG("Setting cgroup for external devices (if required)");
    if (qemuSetupCgroupForExtDevices(vm, driver) < 0)
        goto cleanup;

    VIR_DEBUG("Setting up resctrl");
    if (qemuProcessResctrlCreate(driver, vm) < 0)
        goto cleanup;

    VIR_DEBUG("Setting up managed PR daemon");
    if (virDomainDefHasManagedPR(vm->def) &&
        qemuProcessStartManagedPRDaemon(vm) < 0)
        goto cleanup;

    VIR_DEBUG("Setting domain security labels");
    if (qemuSecuritySetAllLabel(driver,
                                vm,
                                incoming ? incoming->path : NULL,
                                incoming != NULL) < 0)
        goto cleanup;

    /* Security manager labeled all devices, therefore
     * if any operation from now on fails, we need to ask the caller to
     * restore labels.
     */
    ret = -2;

    if (incoming && incoming->fd != -1) {
        /* if there's an fd to migrate from, and it's a pipe, put the
         * proper security label on it
         */
        struct stat stdin_sb;

        VIR_DEBUG("setting security label on pipe used for migration");

        if (fstat(incoming->fd, &stdin_sb) < 0) {
            virReportSystemError(errno,
                                 _("cannot stat fd %d"), incoming->fd);
            goto cleanup;
        }
        if (S_ISFIFO(stdin_sb.st_mode) &&
            qemuSecuritySetImageFDLabel(driver->securityManager,
                                        vm->def, incoming->fd) < 0)
            goto cleanup;
    }

    VIR_DEBUG("Labelling done, completing handshake to child");
    if (virCommandHandshakeNotify(cmd) < 0)
        goto cleanup;
    VIR_DEBUG("Handshake complete, child running");

    if (qemuDomainObjStartWorker(vm) < 0)
        goto cleanup;

    VIR_DEBUG("Waiting for monitor to show up");
    if (qemuProcessWaitForMonitor(driver, vm, asyncJob, logCtxt) < 0)
        goto cleanup;

    if (qemuConnectAgent(driver, vm) < 0)
        goto cleanup;

    VIR_DEBUG("Verifying and updating provided guest CPU");
    if (qemuProcessUpdateAndVerifyCPU(driver, vm, asyncJob) < 0)
        goto cleanup;

    VIR_DEBUG("setting up hotpluggable cpus");
    if (qemuDomainHasHotpluggableStartupVcpus(vm->def)) {
        if (qemuDomainRefreshVcpuInfo(driver, vm, asyncJob, false) < 0)
            goto cleanup;

        if (qemuProcessValidateHotpluggableVcpus(vm->def) < 0)
            goto cleanup;

        if (qemuProcessSetupHotpluggableVcpus(driver, vm, asyncJob) < 0)
            goto cleanup;
    }

    VIR_DEBUG("Refreshing VCPU info");
    if (qemuDomainRefreshVcpuInfo(driver, vm, asyncJob, false) < 0)
        goto cleanup;

    if (qemuDomainValidateVcpuInfo(vm) < 0)
        goto cleanup;

    qemuDomainVcpuPersistOrder(vm->def);

    VIR_DEBUG("Detecting IOThread PIDs");
    if (qemuProcessDetectIOThreadPIDs(driver, vm, asyncJob) < 0)
        goto cleanup;

    VIR_DEBUG("Setting global CPU cgroup (if required)");
    if (qemuSetupGlobalCpuCgroup(vm) < 0)
        goto cleanup;

    VIR_DEBUG("Setting vCPU tuning/settings");
    if (qemuProcessSetupVcpus(vm) < 0)
        goto cleanup;

    VIR_DEBUG("Setting IOThread tuning/settings");
    if (qemuProcessSetupIOThreads(vm) < 0)
        goto cleanup;

    VIR_DEBUG("Setting emulator scheduler");
    if (vm->def->cputune.emulatorsched &&
        virProcessSetScheduler(vm->pid,
                               vm->def->cputune.emulatorsched->policy,
                               vm->def->cputune.emulatorsched->priority) < 0)
        goto cleanup;

    VIR_DEBUG("Setting any required VM passwords");
    if (qemuProcessInitPasswords(driver, vm, asyncJob) < 0)
        goto cleanup;

    /* set default link states */
    /* qemu doesn't support setting this on the command line, so
     * enter the monitor */
    VIR_DEBUG("Setting network link states");
    if (qemuProcessSetLinkStates(driver, vm, asyncJob) < 0)
        goto cleanup;

    VIR_DEBUG("Setting initial memory amount");
    if (qemuProcessSetupBalloon(driver, vm, asyncJob) < 0)
        goto cleanup;

    if (qemuProcessSetupDiskThrottlingBlockdev(driver, vm, asyncJob) < 0)
        goto cleanup;

    /* Since CPUs were not started yet, the balloon could not return the memory
     * to the host and thus cur_balloon needs to be updated so that GetXMLdesc
     * and friends return the correct size in case they can't grab the job */
    if (!incoming && !snapshot &&
        qemuProcessRefreshBalloonState(driver, vm, asyncJob) < 0)
        goto cleanup;

    if (flags & VIR_QEMU_PROCESS_START_AUTODESTROY &&
        qemuProcessAutoDestroyAdd(driver, vm, conn) < 0)
        goto cleanup;

    VIR_DEBUG("Setting up transient disk");
    if (qemuSnapshotCreateDisksTransient(vm, asyncJob) < 0)
        goto cleanup;

    ret = 0;

 cleanup:
    qemuDomainSecretDestroy(vm);
    return ret;
}