make_etype_info(krb5_context context, krb5_preauthtype pa_type,
                krb5_principal client, krb5_key_data *client_key,
                krb5_enctype enctype, krb5_pa_data **pa_out)
{
    krb5_error_code retval;
    krb5_etype_info_entry **entry = NULL;
    krb5_data *der_etype_info = NULL;
    int etype_info2 = (pa_type == KRB5_PADATA_ETYPE_INFO2);

    *pa_out = NULL;

    entry = k5calloc(2, sizeof(*entry), &retval);
    if (entry == NULL)
        goto cleanup;
    retval = _make_etype_info_entry(context, client, client_key, enctype,
                                    &entry[0], etype_info2);
    if (retval != 0)
        goto cleanup;

    if (etype_info2)
        retval = encode_krb5_etype_info2(entry, &der_etype_info);
    else
        retval = encode_krb5_etype_info(entry, &der_etype_info);
    if (retval)
        goto cleanup;

    /* Steal the data from der_etype_info to create a pa-data element. */
    retval = alloc_pa_data(pa_type, 0, pa_out);
    if (retval)
        goto cleanup;
    (*pa_out)->contents = (uint8_t *)der_etype_info->data;
    (*pa_out)->length = der_etype_info->length;
    der_etype_info->data = NULL;

cleanup:
    krb5_free_etype_info(context, entry);
    krb5_free_data(context, der_etype_info);
    return retval;
}