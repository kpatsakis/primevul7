_pure_ static const char *unit_get_status_message_format_try_harder(Unit *u, JobType t) {
        const char *format;

        assert(u);
        assert(t >= 0);
        assert(t < _JOB_TYPE_MAX);

        format = unit_get_status_message_format(u, t);
        if (format)
                return format;

        /* Return generic strings */
        if (t == JOB_START)
                return "Starting %s.";
        else if (t == JOB_STOP)
                return "Stopping %s.";
        else if (t == JOB_RELOAD)
                return "Reloading %s.";

        return NULL;
}