sync_cookie2str(Sync_Cookie *cookie)
{
    char *cookiestr = NULL;

    if (cookie) {
        if (cookie->openldap_compat) {
            char buf[16] = {0};
            sync_ulong2olcsn(cookie->cookie_change_info, buf);
            cookiestr = slapi_ch_smprintf("%s,csn=%s.000000Z#000000#000#000000",
                                          cookie->cookie_client_signature,
                                          buf);
        } else {
            cookiestr = slapi_ch_smprintf("%s#%s#%lu",
                                          cookie->cookie_server_signature,
                                          cookie->cookie_client_signature,
                                          cookie->cookie_change_info);
        }
    }
    return (cookiestr);
}