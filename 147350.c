qemuProcessFiltersInstantiate(virDomainDefPtr def)
{
    size_t i;

    for (i = 0; i < def->nnets; i++) {
        virDomainNetDefPtr net = def->nets[i];
        if ((net->filter) && (net->ifname)) {
            if (virDomainConfNWFilterInstantiate(def->name, def->uuid, net,
                                                 true) < 0) {
                VIR_WARN("filter '%s' instantiation for '%s' failed '%s'",
                         net->filter, net->ifname, virGetLastErrorMessage());
                virResetLastError();
            }
        }
    }
}