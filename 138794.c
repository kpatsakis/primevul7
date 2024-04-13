check_freshness_token(krb5_context context, krb5_kdcpreauth_rock rock,
                      const krb5_data *token)
{
    krb5_timestamp token_ts, now;
    krb5_key_data *kd;
    krb5_keyblock kb;
    krb5_kvno token_kvno;
    krb5_checksum cksum;
    krb5_data d;
    uint8_t *token_cksum;
    size_t token_cksum_len;
    krb5_boolean valid = FALSE;
    char ckbuf[4];

    memset(&kb, 0, sizeof(kb));

    if (krb5_timeofday(context, &now) != 0)
        goto cleanup;

    if (token->length <= 8)
        goto cleanup;
    token_ts = load_32_be(token->data);
    token_kvno = load_32_be(token->data + 4);
    token_cksum = (uint8_t *)token->data + 8;
    token_cksum_len = token->length - 8;

    /* Check if the token timestamp is too old. */
    if (ts_after(now, ts_incr(token_ts, FRESHNESS_LIFETIME)))
        goto cleanup;

    /* Fetch and decrypt the local krbtgt key of the token's kvno. */
    if (krb5_dbe_find_enctype(context, rock->local_tgt, -1, -1, token_kvno,
                              &kd) != 0)
        goto cleanup;
    if (krb5_dbe_decrypt_key_data(context, NULL, kd, &kb, NULL) != 0)
        goto cleanup;

    /* Verify the token checksum against the current KDC time.  The checksum
     * must use the mandatory checksum type of the krbtgt key's enctype. */
    store_32_be(token_ts, ckbuf);
    d = make_data(ckbuf, sizeof(ckbuf));
    cksum.magic = KV5M_CHECKSUM;
    cksum.checksum_type = 0;
    cksum.length = token_cksum_len;
    cksum.contents = token_cksum;
    (void)krb5_c_verify_checksum(context, &kb, KRB5_KEYUSAGE_PA_AS_FRESHNESS,
                                 &d, &cksum, &valid);

cleanup:
    krb5_free_keyblock_contents(context, &kb);
    return valid ? 0 : KRB5KDC_ERR_PREAUTH_EXPIRED;
}