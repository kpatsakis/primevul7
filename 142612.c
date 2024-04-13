snmp_reset_var_types(netsnmp_variable_list * vbl, u_char new_type)
{
    while (vbl) {
        snmp_set_var_typed_value(vbl, new_type, NULL, 0);
        vbl = vbl->next_variable;
    }
}