fast_armor(krb5_context context, krb5_kdcpreauth_rock rock)
{
    return rock->rstate->armor_key;
}