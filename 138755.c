add_pw_salt(krb5_context context, krb5_kdcpreauth_rock rock,
            krb5_pa_data ***pa_list)
{
    krb5_error_code ret;
    krb5_pa_data *pa;
    krb5_data *salt = NULL;
    krb5_int16 salttype;

    /* Only include this pa-data for old clients. */
    if (rock->client_key == NULL || requires_info2(rock->request))
        return 0;

    ret = krb5_dbe_compute_salt(context, rock->client_key,
                                rock->request->client, &salttype, &salt);
    if (ret)
        return 0;

    if (salttype == KRB5_KDB_SALTTYPE_AFS3) {
        ret = alloc_pa_data(KRB5_PADATA_AFS3_SALT, salt->length + 1, &pa);
        if (ret)
            goto cleanup;
        memcpy(pa->contents, salt->data, salt->length);
        pa->contents[salt->length] = '\0';
    } else {
        /* Steal memory from salt to make the pa-data entry. */
        ret = alloc_pa_data(KRB5_PADATA_PW_SALT, 0, &pa);
        if (ret)
            goto cleanup;
        pa->length = salt->length;
        pa->contents = (uint8_t *)salt->data;
        salt->data = NULL;
    }

    /* add_pa_data_element() claims pa on success or failure. */
    ret = add_pa_data_element(pa_list, pa);

cleanup:
    krb5_free_data(context, salt);
    return ret;
}