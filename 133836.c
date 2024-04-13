void server_process_syslog_message(
        Server *s,
        const char *buf,
        const struct ucred *ucred,
        const struct timeval *tv,
        const char *label,
        size_t label_len) {

        char syslog_priority[sizeof("PRIORITY=") + DECIMAL_STR_MAX(int)],
             syslog_facility[sizeof("SYSLOG_FACILITY") + DECIMAL_STR_MAX(int)];
        const char *message = NULL, *syslog_identifier = NULL, *syslog_pid = NULL;
        struct iovec iovec[N_IOVEC_META_FIELDS + 6];
        unsigned n = 0;
        int priority = LOG_USER | LOG_INFO;
        _cleanup_free_ char *identifier = NULL, *pid = NULL;
        const char *orig;

        assert(s);
        assert(buf);

        orig = buf;
        syslog_parse_priority(&buf, &priority, true);

        if (s->forward_to_syslog)
                forward_syslog_raw(s, priority, orig, ucred, tv);

        syslog_skip_date((char**) &buf);
        syslog_parse_identifier(&buf, &identifier, &pid);

        if (s->forward_to_kmsg)
                server_forward_kmsg(s, priority, identifier, buf, ucred);

        if (s->forward_to_console)
                server_forward_console(s, priority, identifier, buf, ucred);

        if (s->forward_to_wall)
                server_forward_wall(s, priority, identifier, buf, ucred);

        IOVEC_SET_STRING(iovec[n++], "_TRANSPORT=syslog");

        sprintf(syslog_priority, "PRIORITY=%i", priority & LOG_PRIMASK);
        IOVEC_SET_STRING(iovec[n++], syslog_priority);

        if (priority & LOG_FACMASK) {
                sprintf(syslog_facility, "SYSLOG_FACILITY=%i", LOG_FAC(priority));
                IOVEC_SET_STRING(iovec[n++], syslog_facility);
        }

        if (identifier) {
                syslog_identifier = strjoina("SYSLOG_IDENTIFIER=", identifier);
                if (syslog_identifier)
                        IOVEC_SET_STRING(iovec[n++], syslog_identifier);
        }

        if (pid) {
                syslog_pid = strjoina("SYSLOG_PID=", pid);
                if (syslog_pid)
                        IOVEC_SET_STRING(iovec[n++], syslog_pid);
        }

        message = strjoina("MESSAGE=", buf);
        if (message)
                IOVEC_SET_STRING(iovec[n++], message);

        server_dispatch_message(s, iovec, n, ELEMENTSOF(iovec), ucred, tv, label, label_len, NULL, priority, 0);
}