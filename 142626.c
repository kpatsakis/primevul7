snmp_split_pdu(netsnmp_pdu *pdu, int skip_count, int copy_count)
{
    netsnmp_pdu    *newpdu;
    newpdu = _clone_pdu_header(pdu);
    newpdu = _copy_pdu_vars(pdu, newpdu, 0,     /* don't drop any variables */
                            skip_count, copy_count);

    return newpdu;
}