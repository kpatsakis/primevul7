send_freshness_token(krb5_context context, krb5_kdcpreauth_rock rock)
{
    rock->send_freshness_token = TRUE;
}