SPICE_GNUC_VISIBLE int spice_server_add_client(SpiceServer *reds, int socket, int skip_auth)
{
    RedLinkInfo *link;

    if (!(link = reds_init_client_connection(reds, socket))) {
        spice_warning("accept failed");
        return -1;
    }

    link->skip_auth = skip_auth;

    reds_handle_new_link(link);
    return 0;
}