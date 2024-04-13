set_cookie(krb5_context context, krb5_kdcpreauth_rock rock,
           krb5_preauthtype pa_type, const krb5_data *data)
{
    return kdc_fast_set_cookie(rock->rstate, pa_type, data);
}