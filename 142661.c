snmp_add_null_var(netsnmp_pdu *pdu, const oid * name, size_t name_length)
{
    return snmp_pdu_add_variable(pdu, name, name_length, ASN_NULL, NULL, 0);
}