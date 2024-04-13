netsnmp_query_set_default_session( netsnmp_session *sess) {
    DEBUGMSGTL(("iquery", "set default session %p\n", sess));
    _def_query_session = sess;
}