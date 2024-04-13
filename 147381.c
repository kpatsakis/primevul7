qemuProcessHandleShutdown(qemuMonitorPtr mon G_GNUC_UNUSED,
                          virDomainObjPtr vm,
                          virTristateBool guest_initiated,
                          void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    qemuDomainObjPrivatePtr priv;
    virObjectEventPtr event = NULL;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);
    int detail = 0;

    VIR_DEBUG("vm=%p", vm);

    virObjectLock(vm);

    priv = vm->privateData;
    if (virDomainObjGetState(vm, NULL) == VIR_DOMAIN_SHUTDOWN) {
        VIR_DEBUG("Ignoring repeated SHUTDOWN event from domain %s",
                  vm->def->name);
        goto unlock;
    } else if (!virDomainObjIsActive(vm)) {
        VIR_DEBUG("Ignoring SHUTDOWN event from inactive domain %s",
                  vm->def->name);
        goto unlock;
    }

    /* In case of fake reboot qemu shutdown state is transient so don't
     * change domain state nor send events. */
    if (!priv->fakeReboot) {
        VIR_DEBUG("Transitioned guest %s to shutdown state",
                  vm->def->name);
        virDomainObjSetState(vm,
                             VIR_DOMAIN_SHUTDOWN,
                             VIR_DOMAIN_SHUTDOWN_UNKNOWN);

        switch (guest_initiated) {
        case VIR_TRISTATE_BOOL_YES:
            detail = VIR_DOMAIN_EVENT_SHUTDOWN_GUEST;
            break;

        case VIR_TRISTATE_BOOL_NO:
            detail = VIR_DOMAIN_EVENT_SHUTDOWN_HOST;
            break;

        case VIR_TRISTATE_BOOL_ABSENT:
        case VIR_TRISTATE_BOOL_LAST:
        default:
            detail = VIR_DOMAIN_EVENT_SHUTDOWN_FINISHED;
            break;
        }

        event = virDomainEventLifecycleNewFromObj(vm,
                                                  VIR_DOMAIN_EVENT_SHUTDOWN,
                                                  detail);

        if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0) {
            VIR_WARN("Unable to save status on vm %s after state change",
                     vm->def->name);
        }
    } else {
        priv->pausedShutdown = true;
    }

    if (priv->agent)
        qemuAgentNotifyEvent(priv->agent, QEMU_AGENT_EVENT_SHUTDOWN);

    qemuProcessShutdownOrReboot(driver, vm);

 unlock:
    virObjectUnlock(vm);
    virObjectEventStateQueue(driver->domainEventState, event);

    return 0;
}