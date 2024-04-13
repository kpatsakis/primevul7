SPICE_GNUC_VISIBLE int spice_server_migrate_start(SpiceServer *reds)
{
    spice_debug("trace");
    if (!reds->config->mig_spice) {
        return -1;
    }
    return 0;
}