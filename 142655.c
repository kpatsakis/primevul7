netsnmp_query_get_default_session_unchecked( void ) {
    DEBUGMSGTL(("iquery", "get default session %p\n", _def_query_session));
    return _def_query_session;
}