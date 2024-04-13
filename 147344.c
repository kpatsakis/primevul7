qemuProcessPrepareMonitorChr(virDomainChrSourceDefPtr monConfig,
                             const char *domainDir)
{
    monConfig->type = VIR_DOMAIN_CHR_TYPE_UNIX;
    monConfig->data.nix.listen = true;

    monConfig->data.nix.path = g_strdup_printf("%s/monitor.sock", domainDir);
    return 0;
}