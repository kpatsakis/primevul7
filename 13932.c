SPICE_GNUC_VISIBLE int spice_server_set_noauth(SpiceServer *s)
{
    memset(s->config->taTicket.password, 0, sizeof(s->config->taTicket.password));
    s->config->ticketing_enabled = FALSE;
    return 0;
}