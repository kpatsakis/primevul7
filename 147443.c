qemuConnectAgent(virQEMUDriverPtr driver, virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    qemuAgentPtr agent = NULL;
    virDomainChrDefPtr config = qemuFindAgentConfig(vm->def);

    if (!config)
        return 0;

    if (priv->agent)
        return 0;

    if (virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_VSERPORT_CHANGE) &&
        config->state != VIR_DOMAIN_CHR_DEVICE_STATE_CONNECTED) {
        VIR_DEBUG("Deferring connecting to guest agent");
        return 0;
    }

    if (qemuSecuritySetDaemonSocketLabel(driver->securityManager, vm->def) < 0) {
        VIR_ERROR(_("Failed to set security context for agent for %s"),
                  vm->def->name);
        goto cleanup;
    }

    /* Hold an extra reference because we can't allow 'vm' to be
     * deleted while the agent is active */
    virObjectRef(vm);

    virObjectUnlock(vm);

    agent = qemuAgentOpen(vm,
                          config->source,
                          virEventThreadGetContext(priv->eventThread),
                          &agentCallbacks,
                          virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_VSERPORT_CHANGE));

    virObjectLock(vm);

    if (agent == NULL)
        virObjectUnref(vm);

    if (!virDomainObjIsActive(vm)) {
        qemuAgentClose(agent);
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("guest crashed while connecting to the guest agent"));
        return -1;
    }

    if (qemuSecurityClearSocketLabel(driver->securityManager, vm->def) < 0) {
        VIR_ERROR(_("Failed to clear security context for agent for %s"),
                  vm->def->name);
        qemuAgentClose(agent);
        goto cleanup;
    }

    priv->agent = agent;
    if (!priv->agent)
        VIR_INFO("Failed to connect agent for %s", vm->def->name);

 cleanup:
    if (!priv->agent) {
        VIR_WARN("Cannot connect to QEMU guest agent for %s", vm->def->name);
        priv->agentError = true;
        virResetLastError();
    }

    return 0;
}