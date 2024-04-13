qemuProcessHandleDumpCompleted(qemuMonitorPtr mon G_GNUC_UNUSED,
                               virDomainObjPtr vm,
                               int status,
                               qemuMonitorDumpStatsPtr stats,
                               const char *error,
                               void *opaque G_GNUC_UNUSED)
{
    qemuDomainObjPrivatePtr priv;
    qemuDomainJobPrivatePtr jobPriv;

    virObjectLock(vm);

    VIR_DEBUG("Dump completed for domain %p %s with stats=%p error='%s'",
              vm, vm->def->name, stats, NULLSTR(error));

    priv = vm->privateData;
    jobPriv = priv->job.privateData;
    if (priv->job.asyncJob == QEMU_ASYNC_JOB_NONE) {
        VIR_DEBUG("got DUMP_COMPLETED event without a dump_completed job");
        goto cleanup;
    }
    jobPriv->dumpCompleted = true;
    priv->job.current->stats.dump = *stats;
    priv->job.error = g_strdup(error);

    /* Force error if extracting the DUMP_COMPLETED status failed */
    if (!error && status < 0) {
        priv->job.error = g_strdup(virGetLastErrorMessage());
        priv->job.current->stats.dump.status = QEMU_MONITOR_DUMP_STATUS_FAILED;
    }

    virDomainObjBroadcast(vm);

 cleanup:
    virResetLastError();
    virObjectUnlock(vm);
    return 0;
}