get_realm_names(struct server_handle *handle, const char ***list_out)
{
    const char **list;
    int i;

    list = calloc(handle->kdc_numrealms + 1, sizeof(*list));
    if (list == NULL)
        return ENOMEM;
    for (i = 0; i < handle->kdc_numrealms; i++)
        list[i] = handle->kdc_realmlist[i]->realm_name;
    list[i] = NULL;
    *list_out = list;
    return 0;
}