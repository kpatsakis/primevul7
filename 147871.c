snmpConstructReponse(SnmpRequest * rq)
{

    struct snmp_pdu *RespPDU;

    debugs(49, 5, "snmpConstructReponse: Called.");

    if (UsingSmp() && IamWorkerProcess()) {
        AsyncJob::Start(new Snmp::Forwarder(static_cast<Snmp::Pdu&>(*rq->PDU),
                                            static_cast<Snmp::Session&>(rq->session), rq->sock, rq->from));
        snmp_free_pdu(rq->PDU);
        return;
    }

    RespPDU = snmpAgentResponse(rq->PDU);
    snmp_free_pdu(rq->PDU);

    if (RespPDU != NULL) {
        snmp_build(&rq->session, RespPDU, rq->outbuf, &rq->outlen);
        comm_udp_sendto(rq->sock, rq->from, rq->outbuf, rq->outlen);
        snmp_free_pdu(RespPDU);
    }
}