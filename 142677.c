int netsnmp_query_get(netsnmp_variable_list *list,
                      netsnmp_session       *session){
    return _query( list, SNMP_MSG_GET, session );
}