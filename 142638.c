snmp_clone_varbind(netsnmp_variable_list * varlist)
{
    return _copy_varlist(varlist, 0, 10000);    /* skip none, copy all */
}