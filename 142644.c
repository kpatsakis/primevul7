int netsnmp_query_getnext(netsnmp_variable_list *list,
                          netsnmp_session       *session){
    return _query( list, SNMP_MSG_GETNEXT, session );
}