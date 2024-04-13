match_client(krb5_context context, krb5_kdcpreauth_rock rock,
             krb5_principal princ)
{
    krb5_db_entry *ent;
    krb5_boolean match = FALSE;
    krb5_principal req_client = rock->request->client;
    krb5_principal client = rock->client->princ;

    /* Check for a direct match against the request principal or
     * the post-canon client principal. */
    if (krb5_principal_compare_flags(context, princ, req_client,
                                     KRB5_PRINCIPAL_COMPARE_ENTERPRISE) ||
        krb5_principal_compare(context, princ, client))
        return TRUE;

    if (krb5_db_get_principal(context, princ, KRB5_KDB_FLAG_ALIAS_OK, &ent))
        return FALSE;
    match = krb5_principal_compare(context, ent->princ, client);
    krb5_db_free_principal(context, ent);
    return match;
}