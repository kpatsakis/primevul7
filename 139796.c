free_princ_list(krb5_context context, krb5_principal *plist)
{
    size_t i;

    if (plist == NULL)
        return;
    for (i = 0; plist[i] != NULL; i++)
        krb5_free_principal(context, plist[i]);
    free(plist);
}