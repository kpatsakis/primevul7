get_string(krb5_context context, krb5_kdcpreauth_rock rock, const char *key,
           char **value_out)
{
    return krb5_dbe_get_string(context, rock->client, key, value_out);
}