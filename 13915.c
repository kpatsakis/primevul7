SPICE_GNUC_VISIBLE int spice_server_migrate_info(SpiceServer *reds, const char* dest,
                                          int port, int secure_port,
                                          const char* cert_subject)
{
    spice_debug("trace");
    spice_assert(!reds->migration_interface);

    if (!reds_set_migration_dest_info(reds, dest, port, secure_port, cert_subject)) {
        return -1;
    }
    return 0;
}