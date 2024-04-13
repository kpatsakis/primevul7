_pure_ static const char* unit_get_status_message_format(Unit *u, JobType t) {
        const UnitStatusMessageFormats *format_table;

        assert(u);
        assert(t >= 0);
        assert(t < _JOB_TYPE_MAX);

        if (t != JOB_START && t != JOB_STOP)
                return NULL;

        format_table = &UNIT_VTABLE(u)->status_message_formats;
        if (!format_table)
                return NULL;

        return format_table->starting_stopping[t == JOB_STOP];
}