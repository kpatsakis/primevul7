add_freshness_token(krb5_context context, krb5_kdcpreauth_rock rock,
                    krb5_pa_data ***pa_list)
{
    krb5_error_code ret;
    krb5_timestamp now;
    krb5_key_data *kd;
    krb5_keyblock kb;
    krb5_checksum cksum;
    krb5_data d;
    krb5_pa_data *pa;
    char ckbuf[4];

    memset(&cksum, 0, sizeof(cksum));
    memset(&kb, 0, sizeof(kb));

    if (!rock->send_freshness_token)
        return 0;
    if (krb5int_find_pa_data(context, rock->request->padata,
                             KRB5_PADATA_AS_FRESHNESS) == NULL)
        return 0;

    /* Fetch and decrypt the current local krbtgt key. */
    ret = krb5_dbe_find_enctype(context, rock->local_tgt, -1, -1, 0, &kd);
    if (ret)
        goto cleanup;
    ret = krb5_dbe_decrypt_key_data(context, NULL, kd, &kb, NULL);
    if (ret)
        goto cleanup;

    /* Compute a checksum over the current KDC time. */
    ret = krb5_timeofday(context, &now);
    if (ret)
        goto cleanup;
    store_32_be(now, ckbuf);
    d = make_data(ckbuf, sizeof(ckbuf));
    ret = krb5_c_make_checksum(context, 0, &kb, KRB5_KEYUSAGE_PA_AS_FRESHNESS,
                               &d, &cksum);

    /* Compose a freshness token from the time, krbtgt kvno, and checksum. */
    ret = alloc_pa_data(KRB5_PADATA_AS_FRESHNESS, 8 + cksum.length, &pa);
    if (ret)
        goto cleanup;
    store_32_be(now, pa->contents);
    store_32_be(kd->key_data_kvno, pa->contents + 4);
    memcpy(pa->contents + 8, cksum.contents, cksum.length);

    /* add_pa_data_element() claims pa on success or failure. */
    ret = add_pa_data_element(pa_list, pa);

cleanup:
    krb5_free_keyblock_contents(context, &kb);
    krb5_free_checksum_contents(context, &cksum);
    return ret;
}