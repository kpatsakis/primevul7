si_dbus_read_properties(struct session_info *si)
{
    dbus_bool_t locked_hint, ret;
    DBusMessageIter iter, iter_variant;
    gint type;
    DBusError error;
    DBusMessage *message = NULL;
    DBusMessage *reply = NULL;
    gchar *session_object;
    const gchar *interface, *property;

    if (si->session == NULL)
        return;

    session_object = g_strdup_printf(LOGIND_SESSION_OBJ_TEMPLATE, si->session);
    message = dbus_message_new_method_call(LOGIND_INTERFACE,
                                           session_object,
                                           DBUS_PROPERTIES_INTERFACE,
                                           "Get");
    g_free (session_object);
    if (message == NULL) {
        syslog(LOG_ERR, "Unable to create dbus message");
        goto exit;
    }

    interface = LOGIND_SESSION_INTERFACE;
    property = SESSION_PROP_LOCKED_HINT;
    ret = dbus_message_append_args(message,
                                   DBUS_TYPE_STRING, &interface,
                                   DBUS_TYPE_STRING, &property,
                                   DBUS_TYPE_INVALID);
    if (!ret) {
        syslog(LOG_ERR, "Unable to request locked-hint");
        goto exit;
    }

    dbus_error_init(&error);
    reply = dbus_connection_send_with_reply_and_block(si->dbus.system_connection,
                                                      message,
                                                      -1,
                                                      &error);
    if (reply == NULL) {
        if (dbus_error_is_set(&error)) {
            syslog(LOG_ERR, "Properties.Get failed (locked-hint) due %s", error.message);
            dbus_error_free(&error);
        } else {
            syslog(LOG_ERR, "Properties.Get failed (locked-hint)");
        }
        goto exit;
    }

    dbus_message_iter_init(reply, &iter);
    type = dbus_message_iter_get_arg_type(&iter);
    if (type != DBUS_TYPE_VARIANT) {
        syslog(LOG_ERR, "expected a variant, got a '%c' instead", type);
        goto exit;
    }

    dbus_message_iter_recurse(&iter, &iter_variant);
    type = dbus_message_iter_get_arg_type(&iter_variant);
    if (type != DBUS_TYPE_BOOLEAN) {
        syslog(LOG_ERR, "expected a boolean, got a '%c' instead", type);
        goto exit;
    }
    dbus_message_iter_get_basic(&iter_variant, &locked_hint);

    si->session_locked_hint = (locked_hint) ? TRUE : FALSE;
exit:
    if (reply != NULL) {
        dbus_message_unref(reply);
    }

    if (message != NULL) {
        dbus_message_unref(message);
    }
}