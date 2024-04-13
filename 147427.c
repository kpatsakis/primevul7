qemuConnectMonitor(virQEMUDriverPtr driver, virDomainObjPtr vm, int asyncJob,
                   bool retry, qemuDomainLogContextPtr logCtxt)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    qemuMonitorPtr mon = NULL;
    unsigned long long timeout = 0;

    if (qemuSecuritySetDaemonSocketLabel(driver->securityManager, vm->def) < 0) {
        VIR_ERROR(_("Failed to set security context for monitor for %s"),
                  vm->def->name);
        return -1;
    }

    /* When using hugepages, kernel zeroes them out before
     * handing them over to qemu. This can be very time
     * consuming. Therefore, add a second to timeout for each
     * 1GiB of guest RAM. */
    timeout = vm->def->mem.total_memory / (1024 * 1024);

    ignore_value(virTimeMillisNow(&priv->monStart));

    mon = qemuMonitorOpen(vm,
                          priv->monConfig,
                          retry,
                          timeout,
                          virEventThreadGetContext(priv->eventThread),
                          &monitorCallbacks,
                          driver);

    if (mon && logCtxt) {
        g_object_ref(logCtxt);
        qemuMonitorSetDomainLog(mon,
                                qemuProcessMonitorReportLogError,
                                logCtxt,
                                qemuProcessMonitorLogFree);
    }

    priv->monStart = 0;
    priv->mon = mon;

    if (qemuSecurityClearSocketLabel(driver->securityManager, vm->def) < 0) {
        VIR_ERROR(_("Failed to clear security context for monitor for %s"),
                  vm->def->name);
        return -1;
    }

    if (priv->mon == NULL) {
        VIR_INFO("Failed to connect monitor for %s", vm->def->name);
        return -1;
    }

    if (qemuProcessInitMonitor(driver, vm, asyncJob) < 0)
        return -1;

    if (qemuMigrationCapsCheck(driver, vm, asyncJob) < 0)
        return -1;

    return 0;
}