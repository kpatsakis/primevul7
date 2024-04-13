snmp_fix_pdu(netsnmp_pdu *pdu, int command)
{
    netsnmp_pdu    *newpdu;

    if ((pdu->command != SNMP_MSG_RESPONSE)
        || (pdu->errstat == SNMP_ERR_NOERROR)
        || (NULL == pdu->variables)
        || (pdu->errindex > (int)snmp_varbind_len(pdu))
        || (pdu->errindex <= 0)) {
        return NULL;            /* pre-condition tests fail */
    }

    newpdu = _clone_pdu(pdu, 1);        /* copies all except errored variable */
    if (!newpdu)
        return NULL;
    if (!newpdu->variables) {
        snmp_free_pdu(newpdu);
        return NULL;            /* no variables. "should not happen" */
    }
    newpdu->command = command;
    newpdu->reqid = snmp_get_next_reqid();
    newpdu->msgid = snmp_get_next_msgid();
    newpdu->errstat = SNMP_DEFAULT_ERRSTAT;
    newpdu->errindex = SNMP_DEFAULT_ERRINDEX;

    return newpdu;
}