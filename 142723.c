find_varbind_in_list( netsnmp_variable_list *vblist,
                      const oid *name, size_t len)
{
    for (; vblist != NULL; vblist = vblist->next_variable)
        if (!snmp_oid_compare(vblist->name, vblist->name_length, name, len))
            return vblist;

    return NULL;
}