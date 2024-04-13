return_referral_enc_padata( krb5_context context,
                            krb5_enc_kdc_rep_part *reply,
                            krb5_db_entry *server)
{
    krb5_error_code             code;
    krb5_tl_data                tl_data;
    krb5_pa_data                *pa;

    tl_data.tl_data_type = KRB5_TL_SVR_REFERRAL_DATA;
    code = krb5_dbe_lookup_tl_data(context, server, &tl_data);
    if (code || tl_data.tl_data_length == 0)
        return 0;

    code = alloc_pa_data(KRB5_PADATA_SVR_REFERRAL_INFO, tl_data.tl_data_length,
                         &pa);
    if (code)
        return code;
    memcpy(pa->contents, tl_data.tl_data_contents, tl_data.tl_data_length);
    /* add_pa_data_element() claims pa on success or failure. */
    return add_pa_data_element(&reply->enc_padata, pa);
}