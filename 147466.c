qemuProcessUpdateDevices(virQEMUDriverPtr driver,
                         virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    virDomainDeviceDef dev;
    g_auto(GStrv) old = g_steal_pointer(&priv->qemuDevices);
    GStrv tmp;

    if (qemuDomainUpdateDeviceList(driver, vm, QEMU_ASYNC_JOB_NONE) < 0)
        return -1;

    if (!old)
        return 0;

    for (tmp = old; *tmp; tmp++) {
        if (!g_strv_contains((const char **) priv->qemuDevices, *tmp) &&
            virDomainDefFindDevice(vm->def, *tmp, &dev, false) == 0 &&
            qemuDomainRemoveDevice(driver, vm, &dev))
            return -1;
    }

    return 0;
}