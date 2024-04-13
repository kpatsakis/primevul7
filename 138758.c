client_keys(krb5_context context, krb5_kdcpreauth_rock rock,
            krb5_keyblock **keys_out)
{
    krb5_kdc_req *request = rock->request;
    krb5_db_entry *client = rock->client;
    krb5_keyblock *keys, key;
    krb5_key_data *entry_key;
    int i, k;

    keys = calloc(request->nktypes + 1, sizeof(krb5_keyblock));
    if (keys == NULL)
        return ENOMEM;

    k = 0;
    for (i = 0; i < request->nktypes; i++) {
        entry_key = NULL;
        if (krb5_dbe_find_enctype(context, client, request->ktype[i],
                                  -1, 0, &entry_key) != 0)
            continue;
        if (krb5_dbe_decrypt_key_data(context, NULL, entry_key,
                                      &key, NULL) != 0)
            continue;
        keys[k++] = key;
    }
    if (k == 0) {
        free(keys);
        return ENOENT;
    }
    *keys_out = keys;
    return 0;
}