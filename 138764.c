client_name(krb5_context context, krb5_kdcpreauth_rock rock)
{
    return rock->client->princ;
}