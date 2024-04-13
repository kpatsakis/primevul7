free_enginetime_on_shutdown(int majorid, int minorid, void *serverarg,
			    void *clientarg)
{
    u_char engineID[SNMP_MAX_ENG_SIZE];
    size_t engineID_len = sizeof(engineID);

    DEBUGMSGTL(("snmpv3", "free enginetime callback called\n"));

    engineID_len = snmpv3_get_engineID(engineID, engineID_len);
    if (engineID_len > 0)
	free_enginetime(engineID, engineID_len);
    return 0;
}