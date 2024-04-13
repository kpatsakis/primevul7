snmp_clone_pdu(netsnmp_pdu *pdu)
{
    return _clone_pdu(pdu, 0);  /* copies all variables */
}