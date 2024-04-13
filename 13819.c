SPICE_GNUC_VISIBLE int spice_server_add_ssl_client(SpiceServer *reds, int socket, int skip_auth)
{
    RedLinkInfo *link;

    if (!(link = reds_init_client_ssl_connection(reds, socket))) {
        return -1;
    }

    link->skip_auth = skip_auth;
    return 0;
}