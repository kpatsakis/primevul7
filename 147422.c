qemuDomainHasHotpluggableStartupVcpus(virDomainDefPtr def)
{
    size_t maxvcpus = virDomainDefGetVcpusMax(def);
    virDomainVcpuDefPtr vcpu;
    size_t i;

    for (i = 0; i < maxvcpus; i++) {
        vcpu = virDomainDefGetVcpu(def, i);

        if (vcpu->online && vcpu->hotpluggable == VIR_TRISTATE_BOOL_YES)
            return true;
    }

    return false;
}