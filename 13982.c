SPICE_GNUC_VISIBLE int spice_server_set_ticket(SpiceServer *reds,
                                               const char *passwd, int lifetime,
                                               int fail_if_connected,
                                               int disconnect_if_connected)
{
    if (reds_main_channel_connected(reds)) {
        if (fail_if_connected) {
            return -1;
        }
        if (disconnect_if_connected) {
            reds_disconnect(reds);
        }
    }

    on_activating_ticketing(reds);
    reds->config->ticketing_enabled = TRUE;
    if (lifetime == 0) {
        reds->config->taTicket.expiration_time = INT_MAX;
    } else {
        time_t now = spice_get_monotonic_time_ns() / NSEC_PER_SEC;
        reds->config->taTicket.expiration_time = now + lifetime;
    }
    if (passwd != NULL) {
        if (strlen(passwd) > SPICE_MAX_PASSWORD_LENGTH)
            return -1;
        g_strlcpy(reds->config->taTicket.password, passwd, sizeof(reds->config->taTicket.password));
    } else {
        memset(reds->config->taTicket.password, 0, sizeof(reds->config->taTicket.password));
        reds->config->taTicket.expiration_time = 0;
    }
    return 0;
}