static int usm_discover_engineid(struct session_list *slp,
                                 netsnmp_session *session)
{
    netsnmp_pdu    *pdu = NULL, *response = NULL;
    int status, i;

    if (usm_build_probe_pdu(&pdu) != 0) {
        DEBUGMSGTL(("snmp_api", "unable to create probe PDU\n"));
        return SNMP_ERR_GENERR;
    }
    DEBUGMSGTL(("snmp_api", "probing for engineID...\n"));
    session->flags |= SNMP_FLAGS_DONT_PROBE; /* prevent recursion */
    status = snmp_sess_synch_response(slp, pdu, &response);

    if ((response == NULL) && (status == STAT_SUCCESS)) {
        status = STAT_ERROR;
    }

    switch (status) {
    case STAT_SUCCESS:
        session->s_snmp_errno = SNMPERR_INVALID_MSG; /* XX?? */
        DEBUGMSGTL(("snmp_sess_open",
                    "error: expected Report as response to probe: %s (%ld)\n",
                    snmp_errstring(response->errstat),
                    response->errstat));
        break;
    case STAT_ERROR:   /* this is what we expected -> Report == STAT_ERROR */
        session->s_snmp_errno = SNMPERR_UNKNOWN_ENG_ID;
        break;
    case STAT_TIMEOUT:
        session->s_snmp_errno = SNMPERR_TIMEOUT;
        break;
    default:
        DEBUGMSGTL(("snmp_sess_open",
                    "unable to connect with remote engine: %s (%d)\n",
                    snmp_api_errstring(session->s_snmp_errno),
                    session->s_snmp_errno));
        break;
    }

    if (slp->session->securityEngineIDLen == 0) {
        DEBUGMSGTL(("snmp_api",
                    "unable to determine remote engine ID\n"));
        /* clear the flag so that probe occurs on next inform */
        session->flags &= ~SNMP_FLAGS_DONT_PROBE;
        return SNMP_ERR_GENERR;
    }

    session->s_snmp_errno = SNMPERR_SUCCESS;
    if (snmp_get_do_debugging()) {
        DEBUGMSGTL(("snmp_sess_open",
                    "  probe found engineID:  "));
        for (i = 0; i < slp->session->securityEngineIDLen; i++)
            DEBUGMSG(("snmp_sess_open", "%02x",
                      slp->session->securityEngineID[i]));
        DEBUGMSG(("snmp_sess_open", "\n"));
    }

    /*
     * if boot/time supplied set it for this engineID 
     */
    if (session->engineBoots || session->engineTime) {
        set_enginetime(session->securityEngineID,
                       session->securityEngineIDLen,
                       session->engineBoots, session->engineTime,
                       TRUE);
    }
    return SNMPERR_SUCCESS;
}