qemuProcessValidateHotpluggableVcpus(virDomainDefPtr def)
{
    virDomainVcpuDefPtr vcpu;
    virDomainVcpuDefPtr subvcpu;
    qemuDomainVcpuPrivatePtr vcpupriv;
    unsigned int maxvcpus = virDomainDefGetVcpusMax(def);
    size_t i = 0;
    size_t j;
    virBitmapPtr ordermap = virBitmapNew(maxvcpus + 1);
    int ret = -1;

    /* validate:
     * - all hotpluggable entities to be hotplugged have the correct data
     * - vcpus belonging to a hotpluggable entity share configuration
     * - order of the hotpluggable entities is unique
     */
    for (i = 0; i < maxvcpus; i++) {
        vcpu = virDomainDefGetVcpu(def, i);
        vcpupriv = QEMU_DOMAIN_VCPU_PRIVATE(vcpu);

        /* skip over hotpluggable entities  */
        if (vcpupriv->vcpus == 0)
            continue;

        if (vcpu->order != 0) {
            if (virBitmapIsBitSet(ordermap, vcpu->order)) {
                virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                               _("duplicate vcpu order '%u'"), vcpu->order);
                goto cleanup;
            }

            if (virBitmapSetBit(ordermap, vcpu->order)) {
                virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                               _("vcpu order '%u' exceeds vcpu count"),
                               vcpu->order);
                goto cleanup;
            }
        }

        for (j = i + 1; j < (i + vcpupriv->vcpus); j++) {
            subvcpu = virDomainDefGetVcpu(def, j);
            if (subvcpu->hotpluggable != vcpu->hotpluggable ||
                subvcpu->online != vcpu->online ||
                subvcpu->order != vcpu->order) {
                virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                               _("vcpus '%zu' and '%zu' are in the same hotplug "
                                 "group but differ in configuration"), i, j);
                goto cleanup;
            }
        }

        if (vcpu->online && vcpu->hotpluggable == VIR_TRISTATE_BOOL_YES) {
            if ((vcpupriv->socket_id == -1 && vcpupriv->core_id == -1 &&
                 vcpupriv->thread_id == -1 && vcpupriv->node_id == -1) ||
                !vcpupriv->type) {
                virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                               _("vcpu '%zu' is missing hotplug data"), i);
                goto cleanup;
            }
        }
    }

    ret = 0;
 cleanup:
    virBitmapFree(ordermap);
    return ret;
}