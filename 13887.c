SPICE_GNUC_VISIBLE void spice_server_set_addr(SpiceServer *reds, const char *addr, int flags)
{
    g_strlcpy(reds->config->spice_addr, addr, sizeof(reds->config->spice_addr));

    if (flags == SPICE_ADDR_FLAG_IPV4_ONLY) {
        reds->config->spice_family = PF_INET;
    } else if (flags == SPICE_ADDR_FLAG_IPV6_ONLY) {
        reds->config->spice_family = PF_INET6;
    } else if (flags == SPICE_ADDR_FLAG_UNIX_ONLY) {
        reds->config->spice_family = AF_UNIX;
    } else if (flags != 0) {
        spice_warning("unknown address flag: 0x%X", flags);
    }
}