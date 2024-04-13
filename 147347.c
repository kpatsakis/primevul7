qemuProcessReconnectCheckMemAliasOrderMismatch(virDomainObjPtr vm)
{
    size_t i;
    int aliasidx;
    virDomainDefPtr def = vm->def;
    qemuDomainObjPrivatePtr priv = vm->privateData;

    if (!virDomainDefHasMemoryHotplug(def) || def->nmems == 0)
        return;

    for (i = 0; i < def->nmems; i++) {
        aliasidx = qemuDomainDeviceAliasIndex(&def->mems[i]->info, "dimm");

        if (def->mems[i]->info.addr.dimm.slot != aliasidx) {
            priv->memAliasOrderMismatch = true;
            break;
        }
    }
}