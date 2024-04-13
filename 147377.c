qemuProcessHandleResume(qemuMonitorPtr mon G_GNUC_UNUSED,
                        virDomainObjPtr vm,
                        void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr event = NULL;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);
    qemuDomainObjPrivatePtr priv;
    virDomainRunningReason reason = VIR_DOMAIN_RUNNING_UNPAUSED;
    virDomainEventResumedDetailType eventDetail;

    virObjectLock(vm);

    priv = vm->privateData;
    if (priv->runningReason != VIR_DOMAIN_RUNNING_UNKNOWN) {
        reason = priv->runningReason;
        priv->runningReason = VIR_DOMAIN_RUNNING_UNKNOWN;
    }

    if (virDomainObjGetState(vm, NULL) != VIR_DOMAIN_RUNNING) {
        eventDetail = qemuDomainRunningReasonToResumeEvent(reason);
        VIR_DEBUG("Transitioned guest %s into running state, reason '%s', "
                  "event detail %d",
                  vm->def->name, virDomainRunningReasonTypeToString(reason),
                  eventDetail);

        virDomainObjSetState(vm, VIR_DOMAIN_RUNNING, reason);
        event = virDomainEventLifecycleNewFromObj(vm,
                                                  VIR_DOMAIN_EVENT_RESUMED,
                                                  eventDetail);

        if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0) {
            VIR_WARN("Unable to save status on vm %s after state change",
                     vm->def->name);
        }
    }

    virObjectUnlock(vm);
    virObjectEventStateQueue(driver->domainEventState, event);
    return 0;
}