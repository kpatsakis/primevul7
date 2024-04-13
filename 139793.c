add_princ_list(krb5_context context, krb5_const_principal princ,
               krb5_principal **plist)
{
    size_t i;
    krb5_principal *newlist;

    /* Check if princ is already in plist, and count the elements. */
    for (i = 0; (*plist) != NULL && (*plist)[i] != NULL; i++) {
        if (krb5_principal_compare(context, princ, (*plist)[i]))
            return 0;
    }

    newlist = realloc(*plist, (i + 2) * sizeof(*newlist));
    if (newlist == NULL)
        return ENOMEM;
    *plist = newlist;
    newlist[i] = newlist[i + 1] = NULL; /* terminate the list */
    return krb5_copy_principal(context, princ, &newlist[i]);
}