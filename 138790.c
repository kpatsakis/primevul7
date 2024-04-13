max_time_skew(krb5_context context, krb5_kdcpreauth_rock rock)
{
    return context->clockskew;
}