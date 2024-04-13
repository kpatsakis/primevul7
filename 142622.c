snmp_synch_response_cb(netsnmp_session * ss,
                       netsnmp_pdu *pdu,
                       netsnmp_pdu **response, snmp_callback pcb)
{
    struct synch_state    lstate, *state;
    snmp_callback         cbsav;
    void                 *cbmagsav;
    int                   numfds, count;
    netsnmp_large_fd_set  fdset;
    struct timeval        timeout, *tvp;
    int                   block;

    memset((void *) &lstate, 0, sizeof(lstate));
    state = &lstate;
    cbsav = ss->callback;
    cbmagsav = ss->callback_magic;
    ss->callback = pcb;
    ss->callback_magic = (void *) state;
    netsnmp_large_fd_set_init(&fdset, FD_SETSIZE);

    if (snmp_send(ss, pdu) == 0) {
        snmp_free_pdu(pdu);
        state->status = STAT_ERROR;
    } else {
        state->reqid = pdu->reqid;
        state->waiting = 1;
    }

    while (state->waiting) {
        numfds = 0;
        NETSNMP_LARGE_FD_ZERO(&fdset);
        block = NETSNMP_SNMPBLOCK;
        tvp = &timeout;
        timerclear(tvp);
        snmp_sess_select_info2_flags(NULL, &numfds, &fdset, tvp, &block,
                                     NETSNMP_SELECT_NOALARMS);
        if (block == 1)
            tvp = NULL;         /* block without timeout */
        count = netsnmp_large_fd_set_select(numfds, &fdset, NULL, NULL, tvp);
        if (count > 0) {
            snmp_read2(&fdset);
        } else {
            switch (count) {
            case 0:
                snmp_timeout();
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

        if ( ss->flags & SNMP_FLAGS_RESP_CALLBACK ) {
            void (*cb)(void);
            cb = (void (*)(void))(ss->myvoid);
            cb();        /* Used to invoke 'netsnmp_check_outstanding_agent_requests();'
                            on internal AgentX queries.  */
        }
    }
    *response = state->pdu;
    ss->callback = cbsav;
    ss->callback_magic = cbmagsav;
    netsnmp_large_fd_set_cleanup(&fdset);
    return state->status;
}