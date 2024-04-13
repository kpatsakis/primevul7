qemuProcessHandleSpiceMigrated(qemuMonitorPtr mon G_GNUC_UNUSED,
                               virDomainObjPtr vm,
                               void *opaque G_GNUC_UNUSED)
{
    qemuDomainObjPrivatePtr priv;
    qemuDomainJobPrivatePtr jobPriv;

    virObjectLock(vm);

    VIR_DEBUG("Spice migration completed for domain %p %s",
              vm, vm->def->name);

    priv = vm->privateData;
    jobPriv = priv->job.privateData;
    if (priv->job.asyncJob != QEMU_ASYNC_JOB_MIGRATION_OUT) {
        VIR_DEBUG("got SPICE_MIGRATE_COMPLETED event without a migration job");
        goto cleanup;
    }

    jobPriv->spiceMigrated = true;
    virDomainObjBroadcast(vm);

 cleanup:
    virObjectUnlock(vm);
    return 0;
}