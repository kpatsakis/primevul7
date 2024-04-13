static void unit_status_log_starting_stopping_reloading(Unit *u, JobType t) {
        const char *format;
        char buf[LINE_MAX];
        sd_id128_t mid;

        assert(u);

        if (t != JOB_START && t != JOB_STOP && t != JOB_RELOAD)
                return;

        if (log_on_console())
                return;

        /* We log status messages for all units and all operations. */

        format = unit_get_status_message_format_try_harder(u, t);
        if (!format)
                return;

        snprintf(buf, sizeof(buf), format, unit_description(u));
        char_array_0(buf);

        mid = t == JOB_START ? SD_MESSAGE_UNIT_STARTING :
              t == JOB_STOP  ? SD_MESSAGE_UNIT_STOPPING :
                               SD_MESSAGE_UNIT_RELOADING;

        log_struct_unit(LOG_INFO,
                        u->id,
                        MESSAGE_ID(mid),
                        "MESSAGE=%s", buf,
                        NULL);
}