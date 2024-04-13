SPICE_GNUC_VISIBLE void spice_server_set_seamless_migration(SpiceServer *reds, int enable)
{
    /* seamless migration is not supported with multiple clients */
    reds->seamless_migration_enabled = enable && !reds->allow_multiple_clients;
    spice_debug("seamless migration enabled=%d", enable);
}