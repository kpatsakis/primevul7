qemuProcessPrepareDomainNUMAPlacement(virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    g_autofree char *nodeset = NULL;
    g_autoptr(virBitmap) numadNodeset = NULL;
    g_autoptr(virBitmap) hostMemoryNodeset = NULL;
    g_autoptr(virCapsHostNUMA) caps = NULL;

    /* Get the advisory nodeset from numad if 'placement' of
     * either <vcpu> or <numatune> is 'auto'.
     */
    if (!virDomainDefNeedsPlacementAdvice(vm->def))
        return 0;

    nodeset = virNumaGetAutoPlacementAdvice(virDomainDefGetVcpus(vm->def),
                                            virDomainDefGetMemoryTotal(vm->def));

    if (!nodeset)
        return -1;

    if (!(hostMemoryNodeset = virNumaGetHostMemoryNodeset()))
        return -1;

    VIR_DEBUG("Nodeset returned from numad: %s", nodeset);

    if (virBitmapParse(nodeset, &numadNodeset, VIR_DOMAIN_CPUMASK_LEN) < 0)
        return -1;

    if (!(caps = virCapabilitiesHostNUMANewHost()))
        return -1;

    /* numad may return a nodeset that only contains cpus but cgroups don't play
     * well with that. Set the autoCpuset from all cpus from that nodeset, but
     * assign autoNodeset only with nodes containing memory. */
    if (!(priv->autoCpuset = virCapabilitiesHostNUMAGetCpus(caps, numadNodeset)))
        return -1;

    virBitmapIntersect(numadNodeset, hostMemoryNodeset);

    priv->autoNodeset = g_steal_pointer(&numadNodeset);

    return 0;
}