void server_maybe_warn_forward_syslog_missed(Server *s) {
        usec_t n;
        assert(s);

        if (s->n_forward_syslog_missed <= 0)
                return;

        n = now(CLOCK_MONOTONIC);
        if (s->last_warn_forward_syslog_missed + WARN_FORWARD_SYSLOG_MISSED_USEC > n)
                return;

        server_driver_message(s, SD_MESSAGE_FORWARD_SYSLOG_MISSED, "Forwarding to syslog missed %u messages.", s->n_forward_syslog_missed);

        s->n_forward_syslog_missed = 0;
        s->last_warn_forward_syslog_missed = n;
}