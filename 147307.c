void unit_status_printf(Unit *u, const char *status, const char *unit_status_msg_format) {
        manager_status_printf(u->manager, false, status, unit_status_msg_format, unit_description(u));
}