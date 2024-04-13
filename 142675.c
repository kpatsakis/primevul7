netsnmp_query_get_default_session( void ) {
    static int warning_logged = 0;

    if (! _def_query_session && ! warning_logged) {
        if (! netsnmp_ds_get_string(NETSNMP_DS_APPLICATION_ID,
                                    NETSNMP_DS_AGENT_INTERNAL_SECNAME)) {
            snmp_log(LOG_WARNING,
                     "iquerySecName has not been configured - internal queries will fail\n");
        } else {
            snmp_log(LOG_WARNING,
                     "default session is not available - internal queries will fail\n");
        }
        warning_logged = 1;
    }

    return netsnmp_query_get_default_session_unchecked();
}