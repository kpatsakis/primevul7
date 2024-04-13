qemuProcessReconnect(void *opaque)
{
    struct qemuProcessReconnectData *data = opaque;
    virQEMUDriverPtr driver = data->driver;
    virDomainObjPtr obj = data->obj;
    qemuDomainObjPrivatePtr priv;
    g_auto(qemuDomainJobObj) oldjob = {
      .cb = NULL,
    };
    int state;
    int reason;
    g_autoptr(virQEMUDriverConfig) cfg = NULL;
    size_t i;
    unsigned int stopFlags = 0;
    bool jobStarted = false;
    bool retry = true;
    bool tryMonReconn = false;

    virIdentitySetCurrent(data->identity);
    g_clear_object(&data->identity);
    VIR_FREE(data);

    qemuDomainObjRestoreJob(obj, &oldjob);
    if (oldjob.asyncJob == QEMU_ASYNC_JOB_MIGRATION_IN)
        stopFlags |= VIR_QEMU_PROCESS_STOP_MIGRATED;

    cfg = virQEMUDriverGetConfig(driver);
    priv = obj->privateData;

    /* expect that libvirt might have crashed during VM start, so prevent
     * cleanup of transient disks */
    priv->inhibitDiskTransientDelete = true;

    if (qemuDomainObjBeginJob(driver, obj, QEMU_JOB_MODIFY) < 0)
        goto error;
    jobStarted = true;

    /* XXX If we ever gonna change pid file pattern, come up with
     * some intelligence here to deal with old paths. */
    if (!(priv->pidfile = virPidFileBuildPath(cfg->stateDir, obj->def->name)))
        goto error;

    /* Restore the masterKey */
    if (qemuDomainMasterKeyReadFile(priv) < 0)
        goto error;

    /* If we are connecting to a guest started by old libvirt there is no
     * allowReboot in status XML and we need to initialize it. */
    qemuProcessPrepareAllowReboot(obj);

    if (qemuHostdevUpdateActiveDomainDevices(driver, obj->def) < 0)
        goto error;

    if (priv->qemuCaps &&
        virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_CHARDEV_FD_PASS))
        retry = false;

    if (qemuDomainObjStartWorker(obj) < 0)
        goto error;

    VIR_DEBUG("Reconnect monitor to def=%p name='%s' retry=%d",
              obj, obj->def->name, retry);

    tryMonReconn = true;

    /* XXX check PID liveliness & EXE path */
    if (qemuConnectMonitor(driver, obj, QEMU_ASYNC_JOB_NONE, retry, NULL) < 0)
        goto error;

    priv->machineName = qemuDomainGetMachineName(obj);
    if (!priv->machineName)
        goto error;

    if (qemuConnectCgroup(obj) < 0)
        goto error;

    if (qemuDomainPerfRestart(obj) < 0)
        goto error;

    /* recreate the pflash storage sources */
    if (qemuDomainInitializePflashStorageSource(obj) < 0)
        goto error;

    /* XXX: Need to change as long as lock is introduced for
     * qemu_driver->sharedDevices.
     */
    for (i = 0; i < obj->def->ndisks; i++) {
        virDomainDiskDefPtr disk = obj->def->disks[i];
        virDomainDeviceDef dev;

        if (virDomainDiskTranslateSourcePool(disk) < 0)
            goto error;

        /* backing chains need to be refreshed only if they could change */
        if (priv->reconnectBlockjobs != VIR_TRISTATE_BOOL_NO &&
            !virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_BLOCKDEV)) {
            /* This should be the only place that calls
             * qemuDomainDetermineDiskChain with @report_broken == false
             * to guarantee best-effort domain reconnect */
            virStorageSourceBackingStoreClear(disk->src);
            if (qemuDomainDetermineDiskChain(driver, obj, disk, NULL, false) < 0)
                goto error;
        } else {
            VIR_DEBUG("skipping backing chain detection for '%s'", disk->dst);
        }

        dev.type = VIR_DOMAIN_DEVICE_DISK;
        dev.data.disk = disk;
        if (qemuAddSharedDevice(driver, &dev, obj->def->name) < 0)
            goto error;
    }

    for (i = 0; i < obj->def->ngraphics; i++) {
        if (qemuProcessGraphicsReservePorts(obj->def->graphics[i], true) < 0)
            goto error;
    }

    if (qemuProcessUpdateState(driver, obj) < 0)
        goto error;

    state = virDomainObjGetState(obj, &reason);
    if (state == VIR_DOMAIN_SHUTOFF ||
        (state == VIR_DOMAIN_PAUSED &&
         reason == VIR_DOMAIN_PAUSED_STARTING_UP)) {
        VIR_DEBUG("Domain '%s' wasn't fully started yet, killing it",
                  obj->def->name);
        goto error;
    }

    if (!priv->qemuCaps) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("domain '%s' has no capabilities recorded"),
                       obj->def->name);
        goto error;
    }

    /* vm startup complete, we can remove transient disks if required */
    priv->inhibitDiskTransientDelete = false;

    /* In case the domain shutdown while we were not running,
     * we need to finish the shutdown process. And we need to do it after
     * we have virQEMUCaps filled in.
     */
    if (state == VIR_DOMAIN_SHUTDOWN ||
        (state == VIR_DOMAIN_PAUSED &&
         reason == VIR_DOMAIN_PAUSED_SHUTTING_DOWN)) {
        VIR_DEBUG("Finishing shutdown sequence for domain %s",
                  obj->def->name);
        qemuProcessShutdownOrReboot(driver, obj);
        goto cleanup;
    }

    if (qemuProcessBuildDestroyMemoryPaths(driver, obj, NULL, true) < 0)
        goto error;

    if ((qemuDomainAssignAddresses(obj->def, priv->qemuCaps,
                                   driver, obj, false)) < 0) {
        goto error;
    }

    /* if domain requests security driver we haven't loaded, report error, but
     * do not kill the domain
     */
    ignore_value(qemuSecurityCheckAllLabel(driver->securityManager,
                                           obj->def));

    if (qemuProcessRefreshCPU(driver, obj) < 0)
        goto error;

    if (qemuDomainRefreshVcpuInfo(driver, obj, QEMU_ASYNC_JOB_NONE, true) < 0)
        goto error;

    qemuDomainVcpuPersistOrder(obj->def);

    if (qemuProcessDetectIOThreadPIDs(driver, obj, QEMU_ASYNC_JOB_NONE) < 0)
        goto error;

    if (qemuSecurityReserveLabel(driver->securityManager, obj->def, obj->pid) < 0)
        goto error;

    qemuProcessNotifyNets(obj->def);

    qemuProcessFiltersInstantiate(obj->def);

    if (qemuProcessRefreshDisks(driver, obj, QEMU_ASYNC_JOB_NONE) < 0)
        goto error;

    if (!virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_BLOCKDEV) &&
        qemuBlockNodeNamesDetect(driver, obj, QEMU_ASYNC_JOB_NONE) < 0)
        goto error;

    if (qemuRefreshVirtioChannelState(driver, obj, QEMU_ASYNC_JOB_NONE) < 0)
        goto error;

    /* If querying of guest's RTC failed, report error, but do not kill the domain. */
    qemuRefreshRTC(driver, obj);

    if (qemuProcessRefreshBalloonState(driver, obj, QEMU_ASYNC_JOB_NONE) < 0)
        goto error;

    if (qemuProcessRecoverJob(driver, obj, &oldjob, &stopFlags) < 0)
        goto error;

    if (qemuProcessRefreshBlockjobs(driver, obj) < 0)
        goto error;

    if (qemuProcessUpdateDevices(driver, obj) < 0)
        goto error;

    if (qemuRefreshPRManagerState(driver, obj) < 0)
        goto error;

    qemuProcessReconnectCheckMemAliasOrderMismatch(obj);

    if (qemuConnectAgent(driver, obj) < 0)
        goto error;

    for (i = 0; i < obj->def->nresctrls; i++) {
        size_t j = 0;

        if (virResctrlAllocDeterminePath(obj->def->resctrls[i]->alloc,
                                         priv->machineName) < 0)
            goto error;

        for (j = 0; j < obj->def->resctrls[i]->nmonitors; j++) {
            virDomainResctrlMonDefPtr mon = NULL;

            mon = obj->def->resctrls[i]->monitors[j];
            if (virResctrlMonitorDeterminePath(mon->instance,
                                               priv->machineName) < 0)
                goto error;
        }
    }

    /* update domain state XML with possibly updated state in virDomainObj */
    if (virDomainObjSave(obj, driver->xmlopt, cfg->stateDir) < 0)
        goto error;

    /* Run an hook to allow admins to do some magic */
    if (virHookPresent(VIR_HOOK_DRIVER_QEMU)) {
        g_autofree char *xml = qemuDomainDefFormatXML(driver,
                                                          priv->qemuCaps,
                                                          obj->def, 0);
        int hookret;

        hookret = virHookCall(VIR_HOOK_DRIVER_QEMU, obj->def->name,
                              VIR_HOOK_QEMU_OP_RECONNECT, VIR_HOOK_SUBOP_BEGIN,
                              NULL, xml, NULL);

        /*
         * If the script raised an error abort the launch
         */
        if (hookret < 0)
            goto error;
    }

    if (g_atomic_int_add(&driver->nactive, 1) == 0 && driver->inhibitCallback)
        driver->inhibitCallback(true, driver->inhibitOpaque);

 cleanup:
    if (jobStarted) {
        if (!virDomainObjIsActive(obj))
            qemuDomainRemoveInactive(driver, obj);
        qemuDomainObjEndJob(driver, obj);
    } else {
        if (!virDomainObjIsActive(obj))
            qemuDomainRemoveInactiveJob(driver, obj);
    }
    virDomainObjEndAPI(&obj);
    virNWFilterUnlockFilterUpdates();
    virIdentitySetCurrent(NULL);
    return;

 error:
    if (virDomainObjIsActive(obj)) {
        /* We can't get the monitor back, so must kill the VM
         * to remove danger of it ending up running twice if
         * user tries to start it again later.
         *
         * If we cannot get to the monitor when the QEMU command
         * line used -no-shutdown, then we can safely say that the
         * domain crashed; otherwise, if the monitor was started,
         * then we can blame ourselves, else we failed before the
         * monitor started so we don't really know. */
        if (!priv->mon && tryMonReconn &&
            qemuDomainIsUsingNoShutdown(priv))
            state = VIR_DOMAIN_SHUTOFF_CRASHED;
        else if (priv->mon)
            state = VIR_DOMAIN_SHUTOFF_DAEMON;
        else
            state = VIR_DOMAIN_SHUTOFF_UNKNOWN;

        /* If BeginJob failed, we jumped here without a job, let's hope another
         * thread didn't have a chance to start playing with the domain yet
         * (it's all we can do anyway).
         */
        qemuProcessStop(driver, obj, state, QEMU_ASYNC_JOB_NONE, stopFlags);
    }
    goto cleanup;
}