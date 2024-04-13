qemuProcessUpdateLiveGuestCPU(virDomainObjPtr vm,
                              virCPUDataPtr enabled,
                              virCPUDataPtr disabled)
{
    virDomainDefPtr def = vm->def;
    qemuDomainObjPrivatePtr priv = vm->privateData;
    g_autoptr(virCPUDef) orig = NULL;
    int rc;

    if (!enabled)
        return 0;

    if (!def->cpu ||
        (def->cpu->mode == VIR_CPU_MODE_CUSTOM &&
         !def->cpu->model))
        return 0;

    if (!(orig = virCPUDefCopy(def->cpu)))
        return -1;

    if ((rc = virCPUUpdateLive(def->os.arch, def->cpu, enabled, disabled)) < 0) {
        return -1;
    } else if (rc == 0) {
        /* Store the original CPU in priv if QEMU changed it and we didn't
         * get the original CPU via migration, restore, or snapshot revert.
         */
        if (!priv->origCPU && !virCPUDefIsEqual(def->cpu, orig, false))
            priv->origCPU = g_steal_pointer(&orig);

        def->cpu->check = VIR_CPU_CHECK_FULL;
    }

    return 0;
}