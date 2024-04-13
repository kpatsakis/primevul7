qemuProcessStart(virConnectPtr conn,
                 virQEMUDriverPtr driver,
                 virDomainObjPtr vm,
                 virCPUDefPtr updatedCPU,
                 qemuDomainAsyncJob asyncJob,
                 const char *migrateFrom,
                 int migrateFd,
                 const char *migratePath,
                 virDomainMomentObjPtr snapshot,
                 virNetDevVPortProfileOp vmop,
                 unsigned int flags)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    qemuProcessIncomingDefPtr incoming = NULL;
    unsigned int stopFlags;
    bool relabel = false;
    bool relabelSavedState = false;
    int ret = -1;
    int rv;

    VIR_DEBUG("conn=%p driver=%p vm=%p name=%s id=%d asyncJob=%s "
              "migrateFrom=%s migrateFd=%d migratePath=%s "
              "snapshot=%p vmop=%d flags=0x%x",
              conn, driver, vm, vm->def->name, vm->def->id,
              qemuDomainAsyncJobTypeToString(asyncJob),
              NULLSTR(migrateFrom), migrateFd, NULLSTR(migratePath),
              snapshot, vmop, flags);

    virCheckFlagsGoto(VIR_QEMU_PROCESS_START_COLD |
                      VIR_QEMU_PROCESS_START_PAUSED |
                      VIR_QEMU_PROCESS_START_AUTODESTROY |
                      VIR_QEMU_PROCESS_START_GEN_VMID, cleanup);

    if (!migrateFrom && !snapshot)
        flags |= VIR_QEMU_PROCESS_START_NEW;

    if (qemuProcessInit(driver, vm, updatedCPU,
                        asyncJob, !!migrateFrom, flags) < 0)
        goto cleanup;

    if (migrateFrom) {
        incoming = qemuProcessIncomingDefNew(priv->qemuCaps, NULL, migrateFrom,
                                             migrateFd, migratePath);
        if (!incoming)
            goto stop;
    }

    if (qemuProcessPrepareDomain(driver, vm, flags) < 0)
        goto stop;

    if (qemuProcessPrepareHost(driver, vm, flags) < 0)
        goto stop;

    if (migratePath) {
        if (qemuSecuritySetSavedStateLabel(driver->securityManager,
                                           vm->def, migratePath) < 0)
            goto cleanup;
        relabelSavedState = true;
    }

    if ((rv = qemuProcessLaunch(conn, driver, vm, asyncJob, incoming,
                                snapshot, vmop, flags)) < 0) {
        if (rv == -2)
            relabel = true;
        goto stop;
    }
    relabel = true;

    if (incoming) {
        if (incoming->deferredURI &&
            qemuMigrationDstRun(driver, vm, incoming->deferredURI, asyncJob) < 0)
            goto stop;
    } else {
        /* Refresh state of devices from QEMU. During migration this happens
         * in qemuMigrationDstFinish to ensure that state information is fully
         * transferred. */
        if (qemuProcessRefreshState(driver, vm, asyncJob) < 0)
            goto stop;
    }

    if (qemuProcessFinishStartup(driver, vm, asyncJob,
                                 !(flags & VIR_QEMU_PROCESS_START_PAUSED),
                                 incoming ?
                                 VIR_DOMAIN_PAUSED_MIGRATION :
                                 VIR_DOMAIN_PAUSED_USER) < 0)
        goto stop;

    if (!incoming) {
        /* Keep watching qemu log for errors during incoming migration, otherwise
         * unset reporting errors from qemu log. */
        qemuMonitorSetDomainLog(priv->mon, NULL, NULL, NULL);
    }

    ret = 0;

 cleanup:
    if (relabelSavedState &&
        qemuSecurityRestoreSavedStateLabel(driver->securityManager,
                                           vm->def, migratePath) < 0)
        VIR_WARN("failed to restore save state label on %s", migratePath);
    qemuProcessIncomingDefFree(incoming);
    return ret;

 stop:
    stopFlags = 0;
    if (!relabel)
        stopFlags |= VIR_QEMU_PROCESS_STOP_NO_RELABEL;
    if (migrateFrom)
        stopFlags |= VIR_QEMU_PROCESS_STOP_MIGRATED;
    if (priv->mon)
        qemuMonitorSetDomainLog(priv->mon, NULL, NULL, NULL);
    qemuProcessStop(driver, vm, VIR_DOMAIN_SHUTOFF_FAILED, asyncJob, stopFlags);
    goto cleanup;
}