snmp_sess_synch_response(void *sessp,
                         netsnmp_pdu *pdu, netsnmp_pdu **response)
{
    netsnmp_session      *ss;
    struct synch_state    lstate, *state;
    snmp_callback         cbsav;
    void                 *cbmagsav;
    int                   numfds, count;
    netsnmp_large_fd_set  fdset;
    struct timeval        timeout, *tvp;
    int                   block;

    ss = snmp_sess_session(sessp);
    if (ss == NULL) {
        return STAT_ERROR;
    }

    memset((void *) &lstate, 0, sizeof(lstate));
    state = &lstate;
    cbsav = ss->callback;
    cbmagsav = ss->callback_magic;
    ss->callback = snmp_synch_input;
    ss->callback_magic = (void *) state;
    netsnmp_large_fd_set_init(&fdset, FD_SETSIZE);

    if (snmp_sess_send(sessp, pdu) == 0) {
        snmp_free_pdu(pdu);
        state->status = STAT_ERROR;
    } else {
        state->waiting = 1;
        state->reqid = pdu->reqid;
    }

    while (state->waiting) {
        numfds = 0;
        NETSNMP_LARGE_FD_ZERO(&fdset);
        block = NETSNMP_SNMPBLOCK;
        tvp = &timeout;
        timerclear(tvp);
        snmp_sess_select_info2_flags(sessp, &numfds, &fdset, tvp, &block,
                                     NETSNMP_SELECT_NOALARMS);
        if (block == 1)
            tvp = NULL;         /* block without timeout */
        count = netsnmp_large_fd_set_select(numfds, &fdset, NULL, NULL, tvp);
        if (count > 0) {
            snmp_sess_read2(sessp, &fdset);
        } else
            switch (count) {
            case 0:
                snmp_sess_timeout(sessp);
                break;
            case -1:
                if (errno == EINTR) {
                    continue;
                } else {
                    snmp_errno = SNMPERR_GENERR;    /*MTCRITICAL_RESOURCE */
                    /*
                     * CAUTION! if another thread closed the socket(s)
                     * waited on here, the session structure was freed.
                     * It would be nice, but we can't rely on the pointer.
                     * ss->s_snmp_errno = SNMPERR_GENERR;
                     * ss->s_errno = errno;
                     */
                    snmp_set_detail(strerror(errno));
                }
                /* FALLTHRU */
            default:
                state->status = STAT_ERROR;
                state->waiting = 0;
            }
    }
    *response = state->pdu;
    ss->callback = cbsav;
    ss->callback_magic = cbmagsav;
    netsnmp_large_fd_set_cleanup(&fdset);
    return state->status;
}