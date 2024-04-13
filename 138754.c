add_etype_info(krb5_context context, krb5_kdcpreauth_rock rock,
               krb5_pa_data ***pa_list)
{
    krb5_error_code ret;
    krb5_pa_data *pa;

    if (rock->client_key == NULL)
        return 0;

    if (!requires_info2(rock->request)) {
        /* Include PA-ETYPE-INFO only for old clients. */
        ret = make_etype_info(context, KRB5_PADATA_ETYPE_INFO,
                              rock->client->princ, rock->client_key,
                              rock->client_keyblock->enctype, &pa);
        if (ret)
            return ret;
        /* add_pa_data_element() claims pa on success or failure. */
        ret = add_pa_data_element(pa_list, pa);
        if (ret)
            return ret;
    }

    /* Always include PA-ETYPE-INFO2. */
    ret = make_etype_info(context, KRB5_PADATA_ETYPE_INFO2,
                          rock->client->princ, rock->client_key,
                          rock->client_keyblock->enctype, &pa);
    if (ret)
        return ret;
    /* add_pa_data_element() claims pa on success or failure. */
    return add_pa_data_element(pa_list, pa);
}