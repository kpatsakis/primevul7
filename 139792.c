copy_creds_except(krb5_context context, krb5_ccache incc,
                  krb5_ccache outcc, krb5_principal princ)
{
    krb5_error_code ret, ret2;
    krb5_cc_cursor cur;
    krb5_creds creds;

    /* Turn off TC_OPENCLOSE on input ccache. */
    ret = krb5_cc_set_flags(context, incc, 0);
    if (ret)
        return ret;
    ret = krb5_cc_start_seq_get(context, incc, &cur);
    if (ret)
        goto cleanup;

    while (!(ret = krb5_cc_next_cred(context, incc, &cur, &creds))) {
        if (krb5_principal_compare(context, princ, creds.server))
            continue;

        ret = krb5_cc_store_cred(context, outcc, &creds);
        krb5_free_cred_contents(context, &creds);
        if (ret)
            goto cleanup;
    }

    if (ret != KRB5_CC_END)
        goto cleanup;
    ret = 0;

cleanup:
    ret2 = krb5_cc_set_flags(context, incc, KRB5_TC_OPENCLOSE);
    return (ret == 0) ? ret2 : ret;
}