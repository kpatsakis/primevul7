snmp_pdu_create(int command)
{
    netsnmp_pdu    *pdu;

    pdu = (netsnmp_pdu *) calloc(1, sizeof(netsnmp_pdu));
    if (pdu) {
        pdu->version = SNMP_DEFAULT_VERSION;
        pdu->command = command;
        pdu->errstat = SNMP_DEFAULT_ERRSTAT;
        pdu->errindex = SNMP_DEFAULT_ERRINDEX;
        pdu->securityModel = SNMP_DEFAULT_SECMODEL;
        pdu->transport_data = NULL;
        pdu->transport_data_length = 0;
        pdu->securityNameLen = 0;
        pdu->contextNameLen = 0;
        pdu->time = 0;
        pdu->reqid = snmp_get_next_reqid();
        pdu->msgid = snmp_get_next_msgid();
    }
    return pdu;

}