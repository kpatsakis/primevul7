qemuProcessRefreshBlockjobs(virQEMUDriverPtr driver,
                            virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;

    if (virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_BLOCKDEV))
        return qemuBlockJobRefreshJobs(driver, vm);
    else
        return qemuProcessRefreshLegacyBlockjobs(driver, vm);
}