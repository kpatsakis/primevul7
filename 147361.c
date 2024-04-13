qemuProcessRefreshPRManagerState(virDomainObjPtr vm,
                                 GHashTable *info)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    qemuMonitorPRManagerInfoPtr prManagerInfo;
    const char *managedAlias = qemuDomainGetManagedPRAlias();

    if (!(prManagerInfo = virHashLookup(info, managedAlias))) {
        virReportError(VIR_ERR_OPERATION_FAILED,
                       _("missing info on pr-manager %s"),
                       managedAlias);
        return -1;
    }

    priv->prDaemonRunning = prManagerInfo->connected;

    if (!priv->prDaemonRunning &&
        qemuProcessStartManagedPRDaemon(vm) < 0)
        return -1;

    return 0;
}