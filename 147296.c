int unit_add_exec_dependencies(Unit *u, ExecContext *c) {
        int r;

        assert(u);
        assert(c);

        if (c->working_directory) {
                r = unit_require_mounts_for(u, c->working_directory);
                if (r < 0)
                        return r;
        }

        if (c->root_directory) {
                r = unit_require_mounts_for(u, c->root_directory);
                if (r < 0)
                        return r;
        }

        if (c->std_output != EXEC_OUTPUT_KMSG &&
            c->std_output != EXEC_OUTPUT_SYSLOG &&
            c->std_output != EXEC_OUTPUT_JOURNAL &&
            c->std_output != EXEC_OUTPUT_KMSG_AND_CONSOLE &&
            c->std_output != EXEC_OUTPUT_SYSLOG_AND_CONSOLE &&
            c->std_output != EXEC_OUTPUT_JOURNAL_AND_CONSOLE &&
            c->std_error != EXEC_OUTPUT_KMSG &&
            c->std_error != EXEC_OUTPUT_SYSLOG &&
            c->std_error != EXEC_OUTPUT_JOURNAL &&
            c->std_error != EXEC_OUTPUT_KMSG_AND_CONSOLE &&
            c->std_error != EXEC_OUTPUT_JOURNAL_AND_CONSOLE &&
            c->std_error != EXEC_OUTPUT_SYSLOG_AND_CONSOLE)
                return 0;

        /* If syslog or kernel logging is requested, make sure our own
         * logging daemon is run first. */

        if (u->manager->running_as == SYSTEMD_SYSTEM) {
                r = unit_add_dependency_by_name(u, UNIT_AFTER, SPECIAL_JOURNALD_SOCKET, NULL, true);
                if (r < 0)
                        return r;
        }

        return 0;
}