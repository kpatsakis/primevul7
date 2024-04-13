static void reds_mig_release(RedServerConfig *config)
{
    if (config->mig_spice) {
        g_free(config->mig_spice->cert_subject);
        g_free(config->mig_spice->host);
        g_free(config->mig_spice);
        config->mig_spice = NULL;
    }
}