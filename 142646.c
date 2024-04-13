usm_handle_report(struct session_list *slp,
                  netsnmp_transport *transport, netsnmp_session *session,
                  int result, netsnmp_pdu *pdu)
{
    /*
     * handle reportable errors 
     */

    /* this will get in our way */
    usm_free_usmStateReference(pdu->securityStateRef);
    pdu->securityStateRef = NULL;

    switch (result) {
    case SNMPERR_USM_AUTHENTICATIONFAILURE:
    {
        int res = session->s_snmp_errno;
        session->s_snmp_errno = result;
        if (session->callback) {
            session->callback(NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE,
                              session, pdu->reqid, pdu,
                              session->callback_magic);
        }
        session->s_snmp_errno = res;
    }  
    /* fallthrough */
    case SNMPERR_USM_UNKNOWNENGINEID:
    case SNMPERR_USM_UNKNOWNSECURITYNAME:
    case SNMPERR_USM_UNSUPPORTEDSECURITYLEVEL:
    case SNMPERR_USM_NOTINTIMEWINDOW:
    case SNMPERR_USM_DECRYPTIONERROR:

        if (SNMP_CMD_CONFIRMED(pdu->command) ||
            (pdu->command == 0
             && (pdu->flags & SNMP_MSG_FLAG_RPRT_BIT))) {
            netsnmp_pdu    *pdu2;
            int             flags = pdu->flags;

            pdu->flags |= UCD_MSG_FLAG_FORCE_PDU_COPY;
            pdu2 = snmp_clone_pdu(pdu);
            pdu->flags = pdu2->flags = flags;
            snmpv3_make_report(pdu2, result);
            if (0 == snmp_sess_send(slp, pdu2)) {
                snmp_free_pdu(pdu2);
                /*
                 * TODO: indicate error 
                 */
            }
        }
        break;
    }       
}