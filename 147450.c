qemuProcessGraphicsSetupNetworkAddress(virDomainGraphicsListenDefPtr glisten,
                                       const char *listenAddr)
{
    int rc;

    /* TODO: reject configuration without network specified for network listen */
    if (!glisten->network) {
        glisten->address = g_strdup(listenAddr);
        return 0;
    }

    rc = qemuProcessGetNetworkAddress(glisten->network, &glisten->address);
    if (rc <= -2) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                       _("network-based listen isn't possible, "
                         "network driver isn't present"));
        return -1;
    }
    if (rc < 0)
        return -1;

    return 0;
}