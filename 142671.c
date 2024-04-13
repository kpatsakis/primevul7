snmp_set_var_typed_integer(netsnmp_variable_list * newvar,
                           u_char type, long val)
{
    newvar->type = type;
    return snmp_set_var_value(newvar, &val, sizeof(long));
}