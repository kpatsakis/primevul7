qemuProcessQMPLaunch(qemuProcessQMPPtr proc)
{
    const char *machine;
    int status = 0;
    int rc;

    if (proc->forceTCG)
        machine = "none,accel=tcg";
    else
        machine = "none,accel=kvm:tcg";

    VIR_DEBUG("Try to probe capabilities of '%s' via QMP, machine %s",
              proc->binary, machine);

    /*
     * We explicitly need to use -daemonize here, rather than
     * virCommandDaemonize, because we need to synchronize
     * with QEMU creating its monitor socket API. Using
     * daemonize guarantees control won't return to libvirt
     * until the socket is present.
     */
    proc->cmd = virCommandNewArgList(proc->binary,
                                     "-S",
                                     "-no-user-config",
                                     "-nodefaults",
                                     "-nographic",
                                     "-machine", machine,
                                     "-qmp", proc->monarg,
                                     "-pidfile", proc->pidfile,
                                     "-daemonize",
                                    NULL);
    virCommandAddEnvPassCommon(proc->cmd);
    virCommandClearCaps(proc->cmd);

#if WITH_CAPNG
    /* QEMU might run into permission issues, e.g. /dev/sev (0600), override
     * them just for the purpose of probing */
    if (geteuid() == 0)
        virCommandAllowCap(proc->cmd, CAP_DAC_OVERRIDE);
#endif

    virCommandSetGID(proc->cmd, proc->runGid);
    virCommandSetUID(proc->cmd, proc->runUid);

    virCommandSetErrorBuffer(proc->cmd, &(proc->stdErr));

    if (virCommandRun(proc->cmd, &status) < 0)
        return -1;

    if (status != 0) {
        VIR_DEBUG("QEMU %s exited with status %d", proc->binary, status);
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("Failed to start QEMU binary %s for probing: %s"),
                       proc->binary,
                       proc->stdErr ? proc->stdErr : _("unknown error"));
        return -1;
    }

    if ((rc = virPidFileReadPath(proc->pidfile, &proc->pid)) < 0) {
        virReportSystemError(-rc, _("Failed to read pidfile %s"), proc->pidfile);
        return -1;
    }

    return 0;
}