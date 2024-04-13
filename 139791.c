get_vfy_cred(krb5_context context, krb5_creds *creds, krb5_principal server,
             krb5_keytab keytab, krb5_ccache *ccache_arg)
{
    krb5_error_code ret;
    krb5_ccache ccache = NULL, retcc = NULL;
    krb5_creds in_creds, *out_creds = NULL;
    krb5_auth_context authcon = NULL;
    krb5_data ap_req = empty_data();

    /* If the creds are for the server principal, we're set, just do a mk_req.
     * Otherwise, do a get_credentials first. */
    if (krb5_principal_compare(context, server, creds->server)) {
        /* Make an ap-req. */
        ret = krb5_mk_req_extended(context, &authcon, 0, NULL, creds, &ap_req);
        if (ret)
            goto cleanup;
    } else {
        /*
         * This is unclean, but it's the easiest way without ripping the
         * library into very small pieces.  store the client's initial cred
         * in a memory ccache, then call the library.  Later, we'll copy
         * everything except the initial cred into the ccache we return to
         * the user.  A clean implementation would involve library
         * internals with a coherent idea of "in" and "out".
         */

        /* Insert the initial cred into the ccache. */
        ret = krb5_cc_new_unique(context, "MEMORY", NULL, &ccache);
        if (ret)
            goto cleanup;
        ret = krb5_cc_initialize(context, ccache, creds->client);
        if (ret)
            goto cleanup;
        ret = krb5_cc_store_cred(context, ccache, creds);
        if (ret)
            goto cleanup;

        /* Get credentials with get_creds. */
        memset(&in_creds, 0, sizeof(in_creds));
        in_creds.client = creds->client;
        in_creds.server = server;
        ret = krb5_timeofday(context, &in_creds.times.endtime);
        if (ret)
            goto cleanup;
        in_creds.times.endtime += 5*60;
        ret = krb5_get_credentials(context, 0, ccache, &in_creds, &out_creds);
        if (ret)
            goto cleanup;

        /* Make an ap-req. */
        ret = krb5_mk_req_extended(context, &authcon, 0, NULL, out_creds,
                                   &ap_req);
        if (ret)
            goto cleanup;
    }

    /* Wipe the auth context created by mk_req. */
    if (authcon) {
        krb5_auth_con_free(context, authcon);
        authcon = NULL;
    }

    /* Verify the ap_req. */
    ret = krb5_rd_req(context, &authcon, &ap_req, server, keytab, NULL, NULL);
    if (ret)
        goto cleanup;

    /* If we get this far, then the verification succeeded.  We can
     * still fail if the library stuff here fails, but that's it. */
    if (ccache_arg != NULL && ccache != NULL) {
        if (*ccache_arg == NULL) {
            ret = krb5_cc_resolve(context, "MEMORY:rd_req2", &retcc);
            if (ret)
                goto cleanup;
            ret = krb5_cc_initialize(context, retcc, creds->client);
            if (ret)
                goto cleanup;
            ret = copy_creds_except(context, ccache, retcc, creds->server);
            if (ret)
                goto cleanup;
            *ccache_arg = retcc;
            retcc = NULL;
        } else {
            ret = copy_creds_except(context, ccache, *ccache_arg, server);
        }
    }

cleanup:
    if (retcc != NULL)
        krb5_cc_destroy(context, retcc);
    if (ccache != NULL)
        krb5_cc_destroy(context, ccache);
    krb5_free_creds(context, out_creds);
    krb5_auth_con_free(context, authcon);
    krb5_free_data_contents(context, &ap_req);
    return ret;
}