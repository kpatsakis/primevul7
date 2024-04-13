qemuProcessHandleMemoryFailure(qemuMonitorPtr mon G_GNUC_UNUSED,
                               virDomainObjPtr vm,
                               qemuMonitorEventMemoryFailurePtr mfp,
                               void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr event = NULL;
    virDomainMemoryFailureRecipientType recipient;
    virDomainMemoryFailureActionType action;
    unsigned int flags = 0;

    switch (mfp->recipient) {
    case QEMU_MONITOR_MEMORY_FAILURE_RECIPIENT_HYPERVISOR:
        recipient = VIR_DOMAIN_EVENT_MEMORY_FAILURE_RECIPIENT_HYPERVISOR;
        break;
    case QEMU_MONITOR_MEMORY_FAILURE_RECIPIENT_GUEST:
        recipient = VIR_DOMAIN_EVENT_MEMORY_FAILURE_RECIPIENT_GUEST;
        break;
    case QEMU_MONITOR_MEMORY_FAILURE_RECIPIENT_LAST:
    default:
        virReportError(VIR_ERR_INVALID_ARG, "%s",
                       _("requested unknown memory failure recipient"));
        return -1;
    }

    switch (mfp->action) {
    case QEMU_MONITOR_MEMORY_FAILURE_ACTION_IGNORE:
        action = VIR_DOMAIN_EVENT_MEMORY_FAILURE_ACTION_IGNORE;
        break;
    case QEMU_MONITOR_MEMORY_FAILURE_ACTION_INJECT:
        action = VIR_DOMAIN_EVENT_MEMORY_FAILURE_ACTION_INJECT;
        break;
    case QEMU_MONITOR_MEMORY_FAILURE_ACTION_FATAL:
        action = VIR_DOMAIN_EVENT_MEMORY_FAILURE_ACTION_FATAL;
        break;
    case QEMU_MONITOR_MEMORY_FAILURE_ACTION_RESET:
        action = VIR_DOMAIN_EVENT_MEMORY_FAILURE_ACTION_RESET;
        break;
    case QEMU_MONITOR_MEMORY_FAILURE_ACTION_LAST:
    default:
        virReportError(VIR_ERR_INVALID_ARG, "%s",
                       _("requested unknown memory failure action"));
        return -1;
    }

    if (mfp->action_required)
        flags |= VIR_DOMAIN_MEMORY_FAILURE_ACTION_REQUIRED;
    if (mfp->recursive)
        flags |= VIR_DOMAIN_MEMORY_FAILURE_RECURSIVE;

    event = virDomainEventMemoryFailureNewFromObj(vm, recipient, action, flags);
    virObjectEventStateQueue(driver->domainEventState, event);

    return 0;
}