si_dbus_read_signals(struct session_info *info)
{
    DBusMessage *message = NULL;

    dbus_connection_read_write(info->connection, 0);
    message = dbus_connection_pop_message(info->connection);
    while (message != NULL) {
        const char *member;

        member = dbus_message_get_member (message);
        if (g_strcmp0(member, SEAT_SIGNAL_ACTIVE_SESSION_CHANGED) == 0) {
            DBusMessageIter iter;
            gint type;
            gchar *session;

            g_clear_pointer(&info->active_session, g_free);

            dbus_message_iter_init(message, &iter);
            type = dbus_message_iter_get_arg_type(&iter);
            /* Session should be an object path, but there is a bug in
               ConsoleKit where it sends a string rather than an object_path
               accept object_path too in case the bug ever gets fixed */
            if (type == DBUS_TYPE_STRING || type == DBUS_TYPE_OBJECT_PATH) {
                dbus_message_iter_get_basic(&iter, &session);
                if (session != NULL && session[0] != '\0') {
                    info->active_session = g_strdup(session);
                    si_dbus_match_rule_update(info);
                } else {
                    syslog(LOG_WARNING, "(console-kit) received invalid session. "
                           "No active-session at the moment");
                }
            } else {
                syslog(LOG_ERR,
                       "ActiveSessionChanged message has unexpected type: '%c'",
                       type);
            }
        } else if (g_strcmp0(member, SESSION_SIGNAL_LOCK) == 0) {
            info->session_is_locked = TRUE;
        } else if (g_strcmp0(member, SESSION_SIGNAL_UNLOCK) == 0) {
            info->session_is_locked = FALSE;
        } else if (g_strcmp0(member, SESSION_SIGNAL_IDLE_HINT_CHANGED) == 0) {
            DBusMessageIter iter;
            gint type;
            dbus_bool_t idle_hint;

            dbus_message_iter_init(message, &iter);
            type = dbus_message_iter_get_arg_type(&iter);
            if (type == DBUS_TYPE_BOOLEAN) {
                dbus_message_iter_get_basic(&iter, &idle_hint);
                info->session_idle_hint = (idle_hint);
            } else {
                syslog(LOG_ERR,
                       "(console-kit) IdleHintChanged has unexpected type: '%c'",
                       type);
            }
        } else {
            if (dbus_message_get_type(message) != DBUS_MESSAGE_TYPE_SIGNAL) {
                syslog(LOG_WARNING, "(console-kit) received non signal message");
            } else if (info->verbose) {
                syslog(LOG_DEBUG, "(console-kit) Signal not handled: %s", member);
            }
        }

        dbus_message_unref(message);
        dbus_connection_read_write(info->connection, 0);
        message = dbus_connection_pop_message(info->connection);
    }
}