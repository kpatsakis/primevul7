qemuProcessVerifyKVMFeatures(virDomainDefPtr def,
                             virCPUDataPtr cpu)
{
    int rc = 0;

    if (def->features[VIR_DOMAIN_FEATURE_PVSPINLOCK] != VIR_TRISTATE_SWITCH_ON)
        return 0;

    rc = virCPUDataCheckFeature(cpu, VIR_CPU_x86_KVM_PV_UNHALT);

    if (rc <= 0) {
        if (rc == 0)
            virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                           _("host doesn't support paravirtual spinlocks"));
        return -1;
    }

    return 0;
}