add_auth_indicator(krb5_context context, krb5_kdcpreauth_rock rock,
                   const char *indicator)
{
    return authind_add(context, indicator, rock->auth_indicators);
}