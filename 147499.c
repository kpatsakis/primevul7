qemuProcessHandleReset(qemuMonitorPtr mon G_GNUC_UNUSED,
                       virDomainObjPtr vm,
                       void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr event;
    qemuDomainObjPrivatePtr priv;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);
    int ret = -1;

    virObjectLock(vm);

    event = virDomainEventRebootNewFromObj(vm);
    priv = vm->privateData;
    if (priv->agent)
        qemuAgentNotifyEvent(priv->agent, QEMU_AGENT_EVENT_RESET);

    if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0)
        VIR_WARN("Failed to save status on vm %s", vm->def->name);

    if (vm->def->onReboot == VIR_DOMAIN_LIFECYCLE_ACTION_DESTROY ||
        vm->def->onReboot == VIR_DOMAIN_LIFECYCLE_ACTION_PRESERVE) {

        if (qemuDomainObjBeginJob(driver, vm, QEMU_JOB_MODIFY) < 0)
            goto cleanup;

        if (!virDomainObjIsActive(vm)) {
            VIR_DEBUG("Ignoring RESET event from inactive domain %s",
                      vm->def->name);
            goto endjob;
        }

        qemuProcessStop(driver, vm, VIR_DOMAIN_SHUTOFF_DESTROYED,
                        QEMU_ASYNC_JOB_NONE, 0);
        virDomainAuditStop(vm, "destroyed");
        qemuDomainRemoveInactive(driver, vm);
     endjob:
        qemuDomainObjEndJob(driver, vm);
    }

    ret = 0;
 cleanup:
    virObjectUnlock(vm);
    virObjectEventStateQueue(driver->domainEventState, event);
    return ret;
}