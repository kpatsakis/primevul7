void server_forward_syslog(Server *s, int priority, const char *identifier, const char *message, const struct ucred *ucred, const struct timeval *tv) {
        struct iovec iovec[5];
        char header_priority[DECIMAL_STR_MAX(priority) + 3], header_time[64],
             header_pid[sizeof("[]: ")-1 + DECIMAL_STR_MAX(pid_t) + 1];
        int n = 0;
        time_t t;
        struct tm *tm;
        char *ident_buf = NULL;

        assert(s);
        assert(priority >= 0);
        assert(priority <= 999);
        assert(message);

        if (LOG_PRI(priority) > s->max_level_syslog)
                return;

        /* First: priority field */
        xsprintf(header_priority, "<%i>", priority);
        IOVEC_SET_STRING(iovec[n++], header_priority);

        /* Second: timestamp */
        t = tv ? tv->tv_sec : ((time_t) (now(CLOCK_REALTIME) / USEC_PER_SEC));
        tm = localtime(&t);
        if (!tm)
                return;
        if (strftime(header_time, sizeof(header_time), "%h %e %T ", tm) <= 0)
                return;
        IOVEC_SET_STRING(iovec[n++], header_time);

        /* Third: identifier and PID */
        if (ucred) {
                if (!identifier) {
                        get_process_comm(ucred->pid, &ident_buf);
                        identifier = ident_buf;
                }

                xsprintf(header_pid, "["PID_FMT"]: ", ucred->pid);

                if (identifier)
                        IOVEC_SET_STRING(iovec[n++], identifier);

                IOVEC_SET_STRING(iovec[n++], header_pid);
        } else if (identifier) {
                IOVEC_SET_STRING(iovec[n++], identifier);
                IOVEC_SET_STRING(iovec[n++], ": ");
        }

        /* Fourth: message */
        IOVEC_SET_STRING(iovec[n++], message);

        forward_syslog_iovec(s, iovec, n, ucred, tv);

        free(ident_buf);
}