nofail(krb5_context context, krb5_verify_init_creds_opt *options,
       krb5_creds *creds)
{
    int val;

    if (options &&
        (options->flags & KRB5_VERIFY_INIT_CREDS_OPT_AP_REQ_NOFAIL))
        return (options->ap_req_nofail != 0);
    if (krb5int_libdefault_boolean(context, &creds->client->realm,
                                   KRB5_CONF_VERIFY_AP_REQ_NOFAIL,
                                   &val) == 0)
        return (val != 0);
    return FALSE;
}