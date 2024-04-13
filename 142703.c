int netsnmp_query_set(netsnmp_variable_list *list,
                      netsnmp_session       *session){
    return _query( list, SNMP_MSG_SET, session );
}