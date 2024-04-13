snmp_varbind_len(netsnmp_pdu *pdu)
{
    register netsnmp_variable_list *vars;
    unsigned long   retVal = 0;
    if (pdu)
        for (vars = pdu->variables; vars; vars = vars->next_variable) {
            retVal++;
        }

    return retVal;
}