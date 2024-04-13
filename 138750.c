request_body(krb5_context context, krb5_kdcpreauth_rock rock)
{
    return rock->inner_body;
}