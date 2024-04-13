static void unit_status_print_starting_stopping(Unit *u, JobType t) {
        const char *format;

        assert(u);

        /* We only print status messages for selected units on
         * selected operations. */

        format = unit_get_status_message_format(u, t);
        if (!format)
                return;

        unit_status_printf(u, "", format);
}