qemuProcessVerifyHypervFeatures(virDomainDefPtr def,
                                virCPUDataPtr cpu)
{
    size_t i;
    int rc;

    for (i = 0; i < VIR_DOMAIN_HYPERV_LAST; i++) {
        g_autofree char *cpuFeature = NULL;

        /* always supported string property */
        if (i == VIR_DOMAIN_HYPERV_VENDOR_ID ||
            i == VIR_DOMAIN_HYPERV_SPINLOCKS)
            continue;

        if (def->hyperv_features[i] != VIR_TRISTATE_SWITCH_ON)
            continue;

        cpuFeature = g_strdup_printf("hv-%s", virDomainHypervTypeToString(i));

        rc = virCPUDataCheckFeature(cpu, cpuFeature);

        if (rc < 0) {
            return -1;
        } else if (rc == 1) {
            if (i == VIR_DOMAIN_HYPERV_STIMER) {
                if (def->hyperv_stimer_direct != VIR_TRISTATE_SWITCH_ON)
                    continue;

                rc = virCPUDataCheckFeature(cpu, VIR_CPU_x86_HV_STIMER_DIRECT);
                if (rc < 0)
                    return -1;
                else if (rc == 1)
                    continue;

                virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                               _("host doesn't support hyperv stimer '%s' feature"),
                               "direct");
                return -1;
            }
            continue;
        }

        switch ((virDomainHyperv) i) {
        case VIR_DOMAIN_HYPERV_RELAXED:
        case VIR_DOMAIN_HYPERV_VAPIC:
            VIR_WARN("host doesn't support hyperv '%s' feature",
                     virDomainHypervTypeToString(i));
            break;

        case VIR_DOMAIN_HYPERV_VPINDEX:
        case VIR_DOMAIN_HYPERV_RUNTIME:
        case VIR_DOMAIN_HYPERV_SYNIC:
        case VIR_DOMAIN_HYPERV_STIMER:
        case VIR_DOMAIN_HYPERV_RESET:
        case VIR_DOMAIN_HYPERV_FREQUENCIES:
        case VIR_DOMAIN_HYPERV_REENLIGHTENMENT:
        case VIR_DOMAIN_HYPERV_TLBFLUSH:
        case VIR_DOMAIN_HYPERV_IPI:
        case VIR_DOMAIN_HYPERV_EVMCS:
            virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                           _("host doesn't support hyperv '%s' feature"),
                           virDomainHypervTypeToString(i));
            return -1;

        /* coverity[dead_error_begin] */
        case VIR_DOMAIN_HYPERV_SPINLOCKS:
        case VIR_DOMAIN_HYPERV_VENDOR_ID:
        case VIR_DOMAIN_HYPERV_LAST:
            break;
        }
    }

    return 0;
}