_make_etype_info_entry(krb5_context context,
                       krb5_principal client_princ, krb5_key_data *client_key,
                       krb5_enctype etype, krb5_etype_info_entry **entry_out,
                       int etype_info2)
{
    krb5_error_code retval;
    krb5_int16 salttype;
    krb5_data *salt = NULL;
    krb5_etype_info_entry *entry = NULL;

    *entry_out = NULL;
    entry = malloc(sizeof(*entry));
    if (entry == NULL)
        return ENOMEM;

    entry->magic = KV5M_ETYPE_INFO_ENTRY;
    entry->etype = etype;
    entry->length = KRB5_ETYPE_NO_SALT;
    entry->salt = NULL;
    entry->s2kparams = empty_data();
    retval = krb5_dbe_compute_salt(context, client_key, client_princ,
                                   &salttype, &salt);
    if (retval)
        goto cleanup;
    if (etype_info2 && salttype == KRB5_KDB_SALTTYPE_AFS3) {
        switch (etype) {
        case ENCTYPE_DES_CBC_CRC:
        case ENCTYPE_DES_CBC_MD4:
        case ENCTYPE_DES_CBC_MD5:
            retval = alloc_data(&entry->s2kparams, 1);
            if (retval)
                goto cleanup;
            entry->s2kparams.data[0] = 1;
            break;
        default:
            break;
        }
    }

    entry->length = salt->length;
    entry->salt = (unsigned char *)salt->data;
    salt->data = NULL;
    *entry_out = entry;
    entry = NULL;

cleanup:
    if (entry != NULL)
        krb5_free_data_contents(context, &entry->s2kparams);
    free(entry);
    krb5_free_data(context, salt);
    return retval;
}