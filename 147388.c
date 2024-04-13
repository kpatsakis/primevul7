qemuProcessReconnectHelper(virDomainObjPtr obj,
                           void *opaque)
{
    virThread thread;
    struct qemuProcessReconnectData *src = opaque;
    struct qemuProcessReconnectData *data;
    g_autofree char *name = NULL;

    /* If the VM was inactive, we don't need to reconnect */
    if (!obj->pid)
        return 0;

    data = g_new0(struct qemuProcessReconnectData, 1);

    memcpy(data, src, sizeof(*data));
    data->obj = obj;
    data->identity = virIdentityGetCurrent();

    virNWFilterReadLockFilterUpdates();

    /* this lock and reference will be eventually transferred to the thread
     * that handles the reconnect */
    virObjectLock(obj);
    virObjectRef(obj);

    name = g_strdup_printf("init-%s", obj->def->name);

    if (virThreadCreateFull(&thread, false, qemuProcessReconnect,
                            name, false, data) < 0) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("Could not create thread. QEMU initialization "
                         "might be incomplete"));
        /* We can't spawn a thread and thus connect to monitor. Kill qemu.
         * It's safe to call qemuProcessStop without a job here since there
         * is no thread that could be doing anything else with the same domain
         * object.
         */
        qemuProcessStop(src->driver, obj, VIR_DOMAIN_SHUTOFF_FAILED,
                        QEMU_ASYNC_JOB_NONE, 0);
        qemuDomainRemoveInactiveJobLocked(src->driver, obj);

        virDomainObjEndAPI(&obj);
        virNWFilterUnlockFilterUpdates();
        g_clear_object(&data->identity);
        VIR_FREE(data);
        return -1;
    }

    return 0;
}