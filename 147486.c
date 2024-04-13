qemuProcessPrepareAllowReboot(virDomainObjPtr vm)
{
    virDomainDefPtr def = vm->def;
    qemuDomainObjPrivatePtr priv = vm->privateData;

    if (priv->allowReboot != VIR_TRISTATE_BOOL_ABSENT)
        return;

    if (def->onReboot == VIR_DOMAIN_LIFECYCLE_ACTION_DESTROY &&
        def->onPoweroff == VIR_DOMAIN_LIFECYCLE_ACTION_DESTROY &&
        (def->onCrash == VIR_DOMAIN_LIFECYCLE_ACTION_DESTROY ||
         def->onCrash == VIR_DOMAIN_LIFECYCLE_ACTION_COREDUMP_DESTROY)) {
        priv->allowReboot = VIR_TRISTATE_BOOL_NO;
    } else {
        priv->allowReboot = VIR_TRISTATE_BOOL_YES;
    }
}