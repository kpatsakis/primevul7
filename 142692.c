snmp_set_var_typed_value(netsnmp_variable_list * newvar, u_char type,
                         const void * val_str, size_t val_len)
{
    newvar->type = type;
    return snmp_set_var_value(newvar, val_str, val_len);
}