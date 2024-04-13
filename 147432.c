qemuProcessPrepareDomainHostdevs(virDomainObjPtr vm,
                                 qemuDomainObjPrivatePtr priv)
{
    size_t i;

    for (i = 0; i < vm->def->nhostdevs; i++) {
        virDomainHostdevDefPtr hostdev = vm->def->hostdevs[i];

        if (qemuDomainPrepareHostdev(hostdev, priv) < 0)
            return -1;
    }

    return 0;
}