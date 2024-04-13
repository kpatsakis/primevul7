static int reds_set_migration_dest_info(RedsState *reds,
                                        const char* dest,
                                        int port, int secure_port,
                                        const char* cert_subject)
{
    RedsMigSpice *spice_migration = NULL;

    reds_mig_release(reds->config);
    if ((port == -1 && secure_port == -1) || !dest) {
        return FALSE;
    }

    spice_migration = g_new0(RedsMigSpice, 1);
    spice_migration->port = port;
    spice_migration->sport = secure_port;
    spice_migration->host = g_strdup(dest);
    if (cert_subject) {
        spice_migration->cert_subject = g_strdup(cert_subject);
    }

    reds->config->mig_spice = spice_migration;

    return TRUE;
}