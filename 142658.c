usm_create_user_from_session_hook(struct session_list *slp,
                                  netsnmp_session *session)
{
    DEBUGMSGTL(("usm", "potentially bootstrapping the USM table from session data\n"));
    return usm_create_user_from_session(session);
}