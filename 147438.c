qemuProcessStartManagedPRDaemon(virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    virQEMUDriverPtr driver = priv->driver;
    g_autoptr(virQEMUDriverConfig) cfg = NULL;
    int errfd = -1;
    g_autofree char *pidfile = NULL;
    g_autofree char *socketPath = NULL;
    pid_t cpid = -1;
    g_autoptr(virCommand) cmd = NULL;
    virTimeBackOffVar timebackoff;
    const unsigned long long timeout = 500000; /* ms */
    int ret = -1;

    cfg = virQEMUDriverGetConfig(driver);

    if (!virFileIsExecutable(cfg->prHelperName)) {
        virReportSystemError(errno, _("'%s' is not a suitable pr helper"),
                             cfg->prHelperName);
        goto cleanup;
    }

    if (!(pidfile = qemuProcessBuildPRHelperPidfilePath(vm)))
        goto cleanup;

    if (!(socketPath = qemuDomainGetManagedPRSocketPath(priv)))
        goto cleanup;

    /* Remove stale socket */
    if (unlink(socketPath) < 0 &&
        errno != ENOENT) {
        virReportSystemError(errno,
                             _("Unable to remove stale socket path: %s"),
                             socketPath);
        goto cleanup;
    }

    if (!(cmd = virCommandNewArgList(cfg->prHelperName,
                                     "-k", socketPath,
                                     NULL)))
        goto cleanup;

    virCommandDaemonize(cmd);
    virCommandSetPidFile(cmd, pidfile);
    virCommandSetErrorFD(cmd, &errfd);

    /* Place the process into the same namespace and cgroup as
     * qemu (so that it shares the same view of the system). */
    virCommandSetPreExecHook(cmd, qemuProcessStartPRDaemonHook, vm);

    if (virCommandRun(cmd, NULL) < 0)
        goto cleanup;

    if (virPidFileReadPath(pidfile, &cpid) < 0) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("pr helper %s didn't show up"),
                       cfg->prHelperName);
        goto cleanup;
    }

    if (virTimeBackOffStart(&timebackoff, 1, timeout) < 0)
        goto cleanup;
    while (virTimeBackOffWait(&timebackoff)) {
        char errbuf[1024] = { 0 };

        if (virFileExists(socketPath))
            break;

        if (virProcessKill(cpid, 0) == 0)
            continue;

        if (saferead(errfd, errbuf, sizeof(errbuf) - 1) < 0) {
            virReportSystemError(errno,
                                 _("pr helper %s died unexpectedly"),
                                 cfg->prHelperName);
        } else {
            virReportError(VIR_ERR_OPERATION_FAILED,
                           _("pr helper died and reported: %s"), errbuf);
        }
        goto cleanup;
    }

    if (!virFileExists(socketPath)) {
        virReportError(VIR_ERR_OPERATION_TIMEOUT, "%s",
                       _("pr helper socked did not show up"));
        goto cleanup;
    }

    if (priv->cgroup &&
        virCgroupAddMachineProcess(priv->cgroup, cpid) < 0)
        goto cleanup;

    if (qemuSecurityDomainSetPathLabel(driver, vm, socketPath, true) < 0)
        goto cleanup;

    priv->prDaemonRunning = true;
    ret = 0;
 cleanup:
    if (ret < 0) {
        virCommandAbort(cmd);
        if (cpid >= 0)
            virProcessKillPainfully(cpid, true);
        if (pidfile)
            unlink(pidfile);
    }
    VIR_FORCE_CLOSE(errfd);
    return ret;
}