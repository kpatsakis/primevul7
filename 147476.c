qemuProcessVerifyCPUFeatures(virDomainDefPtr def,
                             virCPUDataPtr cpu)
{
    int rc;

    rc = virCPUCheckFeature(def->os.arch, def->cpu, "invtsc");

    if (rc < 0) {
        return -1;
    } else if (rc == 1) {
        rc = virCPUDataCheckFeature(cpu, "invtsc");
        if (rc <= 0) {
            if (rc == 0) {
                virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                               _("host doesn't support invariant TSC"));
            }
            return -1;
        }
    }

    return 0;
}