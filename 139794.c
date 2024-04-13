get_host_princs_from_keytab(krb5_context context, krb5_keytab keytab,
                            krb5_principal **princ_list_out)
{
    krb5_error_code ret;
    krb5_kt_cursor cursor;
    krb5_keytab_entry kte;
    krb5_principal *plist = NULL, p;

    *princ_list_out = NULL;

    ret = krb5_kt_start_seq_get(context, keytab, &cursor);
    if (ret)
        goto cleanup;

    while ((ret = krb5_kt_next_entry(context, keytab, &kte, &cursor)) == 0) {
        p = kte.principal;
        if (p->length == 2 && data_eq_string(p->data[0], "host"))
            ret = add_princ_list(context, p, &plist);
        krb5_kt_free_entry(context, &kte);
        if (ret)
            break;
    }
    (void)krb5_kt_end_seq_get(context, keytab, &cursor);
    if (ret == KRB5_KT_END)
        ret = 0;
    if (ret)
        goto cleanup;

    *princ_list_out = plist;
    plist = NULL;

cleanup:
    free_princ_list(context, plist);
    return ret;
}