si_dbus_read_signals(struct session_info *si)
{
    DBusMessage *message = NULL;

    dbus_connection_read_write(si->dbus.system_connection, 0);
    message = dbus_connection_pop_message(si->dbus.system_connection);
    while (message != NULL) {
        const char *member;

        member = dbus_message_get_member (message);
        if (g_strcmp0(member, SESSION_SIGNAL_LOCK) == 0) {
            si->session_is_locked = TRUE;
        } else if (g_strcmp0(member, SESSION_SIGNAL_UNLOCK) == 0) {
            si->session_is_locked = FALSE;
        } else {
            if (dbus_message_get_type(message) != DBUS_MESSAGE_TYPE_SIGNAL) {
                syslog(LOG_WARNING, "(systemd-login) received non signal message");
            } else if (si->verbose) {
                syslog(LOG_DEBUG, "(systemd-login) Signal not handled: %s", member);
            }
        }

        dbus_message_unref(message);
        dbus_connection_read_write(si->dbus.system_connection, 0);
        message = dbus_connection_pop_message(si->dbus.system_connection);
    }
}