static void manager_invoke_notify_message(Manager *m, Unit *u, pid_t pid, char *buf, size_t n) {
        _cleanup_strv_free_ char **tags = NULL;

        assert(m);
        assert(u);
        assert(buf);
        assert(n > 0);

        tags = strv_split(buf, "\n\r");
        if (!tags) {
                log_oom();
                return;
        }

        log_debug_unit(u->id, "Got notification message for unit %s", u->id);

        if (UNIT_VTABLE(u)->notify_message)
                UNIT_VTABLE(u)->notify_message(u, pid, tags);
}