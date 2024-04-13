qemuProcessStartHook(virQEMUDriverPtr driver,
                     virDomainObjPtr vm,
                     virHookQemuOpType op,
                     virHookSubopType subop)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    g_autofree char *xml = NULL;
    int ret;

    if (!virHookPresent(VIR_HOOK_DRIVER_QEMU))
        return 0;

    if (!(xml = qemuDomainDefFormatXML(driver, priv->qemuCaps, vm->def, 0)))
        return -1;

    ret = virHookCall(VIR_HOOK_DRIVER_QEMU, vm->def->name, op, subop,
                      NULL, xml, NULL);

    return ret;
}