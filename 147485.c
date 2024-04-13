qemuProcessResctrlCreate(virQEMUDriverPtr driver,
                         virDomainObjPtr vm)
{
    size_t i = 0;
    g_autoptr(virCaps) caps = NULL;
    qemuDomainObjPrivatePtr priv = vm->privateData;

    if (!vm->def->nresctrls)
        return 0;

    /* Force capability refresh since resctrl info can change
     * XXX: move cache info into virresctrl so caps are not needed */
    caps = virQEMUDriverGetCapabilities(driver, true);
    if (!caps)
        return -1;

    for (i = 0; i < vm->def->nresctrls; i++) {
        size_t j = 0;
        if (virResctrlAllocCreate(caps->host.resctrl,
                                  vm->def->resctrls[i]->alloc,
                                  priv->machineName) < 0)
            return -1;

        for (j = 0; j < vm->def->resctrls[i]->nmonitors; j++) {
            virDomainResctrlMonDefPtr mon = NULL;

            mon = vm->def->resctrls[i]->monitors[j];
            if (virResctrlMonitorCreate(mon->instance,
                                        priv->machineName) < 0)
                return -1;
        }
    }

    return 0;
}