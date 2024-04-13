qemuProcessKillManagedPRDaemon(virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    virErrorPtr orig_err;
    g_autofree char *pidfile = NULL;

    if (!(pidfile = qemuProcessBuildPRHelperPidfilePath(vm))) {
        VIR_WARN("Unable to construct pr-helper pidfile path");
        return;
    }

    virErrorPreserveLast(&orig_err);
    if (virPidFileForceCleanupPath(pidfile) < 0) {
        VIR_WARN("Unable to kill pr-helper process");
    } else {
        priv->prDaemonRunning = false;
    }
    virErrorRestore(&orig_err);
}