snmpClosePorts(void)
{
    if (Comm::IsConnOpen(snmpIncomingConn)) {
        debugs(49, DBG_IMPORTANT, "Closing SNMP receiving port " << snmpIncomingConn->local);
        snmpIncomingConn->close();
    }
    snmpIncomingConn = NULL;

    if (Comm::IsConnOpen(snmpOutgoingConn) && snmpIncomingConn != snmpOutgoingConn) {
        // Perform OUT port closure so as not to step on IN port when sharing a conn.
        debugs(49, DBG_IMPORTANT, "Closing SNMP sending port " << snmpOutgoingConn->local);
        snmpOutgoingConn->close();
    }
    snmpOutgoingConn = NULL;
}