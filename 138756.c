get_cookie(krb5_context context, krb5_kdcpreauth_rock rock,
           krb5_preauthtype pa_type, krb5_data *out)
{
    return kdc_fast_search_cookie(rock->rstate, pa_type, out);
}