_clone_pdu_header(netsnmp_pdu *pdu)
{
    netsnmp_pdu    *newpdu;
    struct snmp_secmod_def *sptr;
    int ret;

    if (!pdu)
        return NULL;

    newpdu = (netsnmp_pdu *) malloc(sizeof(netsnmp_pdu));
    if (!newpdu)
        return NULL;
    memmove(newpdu, pdu, sizeof(netsnmp_pdu));

    /*
     * reset copied pointers if copy fails 
     */
    newpdu->variables = NULL;
    newpdu->enterprise = NULL;
    newpdu->community = NULL;
    newpdu->securityEngineID = NULL;
    newpdu->securityName = NULL;
    newpdu->contextEngineID = NULL;
    newpdu->contextName = NULL;
    newpdu->transport_data = NULL;

    /*
     * copy buffers individually. If any copy fails, all are freed. 
     */
    if (snmp_clone_mem((void **) &newpdu->enterprise, pdu->enterprise,
                       sizeof(oid) * pdu->enterprise_length) ||
        snmp_clone_mem((void **) &newpdu->community, pdu->community,
                       pdu->community_len) ||
        snmp_clone_mem((void **) &newpdu->contextEngineID,
                       pdu->contextEngineID, pdu->contextEngineIDLen)
        || snmp_clone_mem((void **) &newpdu->securityEngineID,
                          pdu->securityEngineID, pdu->securityEngineIDLen)
        || snmp_clone_mem((void **) &newpdu->contextName, pdu->contextName,
                          pdu->contextNameLen)
        || snmp_clone_mem((void **) &newpdu->securityName,
                          pdu->securityName, pdu->securityNameLen)
        || snmp_clone_mem((void **) &newpdu->transport_data,
                          pdu->transport_data,
                          pdu->transport_data_length)) {
        snmp_free_pdu(newpdu);
        return NULL;
    }

    sptr = find_sec_mod(newpdu->securityModel);
    if (sptr && sptr->pdu_clone) {
        /* call security model if it needs to know about this */
        ret = sptr->pdu_clone(pdu, newpdu);
        if (ret) {
            snmp_free_pdu(newpdu);
            return NULL;
        }
    }

    return newpdu;
}