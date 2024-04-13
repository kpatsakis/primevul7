qemuProcessStartUpdateCustomCaps(virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(priv->driver);
    qemuDomainXmlNsDefPtr nsdef = vm->def->namespaceData;
    char **next;
    int tmp;
    size_t i;

    if (cfg->capabilityfilters) {
        for (next = cfg->capabilityfilters; *next; next++) {
            if ((tmp = virQEMUCapsTypeFromString(*next)) < 0) {
                virReportError(VIR_ERR_INTERNAL_ERROR,
                               _("invalid capability_filters capability '%s'"),
                               *next);
                return -1;
            }

            virQEMUCapsClear(priv->qemuCaps, tmp);
        }
    }

    if (nsdef) {
        for (i = 0; i < nsdef->ncapsadd; i++) {
            if ((tmp = virQEMUCapsTypeFromString(nsdef->capsadd[i])) < 0) {
                virReportError(VIR_ERR_INTERNAL_ERROR,
                               _("invalid qemu namespace capability '%s'"),
                               nsdef->capsadd[i]);
                return -1;
            }

            virQEMUCapsSet(priv->qemuCaps, tmp);
        }

        for (i = 0; i < nsdef->ncapsdel; i++) {
            if ((tmp = virQEMUCapsTypeFromString(nsdef->capsdel[i])) < 0) {
                virReportError(VIR_ERR_INTERNAL_ERROR,
                               _("invalid qemu namespace capability '%s'"),
                               nsdef->capsdel[i]);
                return -1;
            }

            virQEMUCapsClear(priv->qemuCaps, tmp);
        }
    }

    return 0;
}