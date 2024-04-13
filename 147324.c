static int manager_setup_kdbus(Manager *m) {
#ifdef ENABLE_KDBUS
        _cleanup_free_ char *p = NULL;
#endif

#ifdef ENABLE_KDBUS
        assert(m);

        if (m->kdbus_fd >= 0)
                return 0;

        m->kdbus_fd = bus_kernel_create_bus(m->running_as == SYSTEMD_SYSTEM ? "system" : "user", m->running_as == SYSTEMD_SYSTEM, &p);
        if (m->kdbus_fd < 0) {
                log_debug("Failed to set up kdbus: %s", strerror(-m->kdbus_fd));
                return m->kdbus_fd;
        }

        log_debug("Successfully set up kdbus on %s", p);

        /* Create the namespace directory here, so that the contents
         * of that directory is not visible to non-root users. This is
         * necessary to ensure that users cannot get access to busses
         * of virtualized users when no UID namespacing is used. */
        if (m->running_as == SYSTEMD_SYSTEM)
                mkdir_p_label("/dev/kdbus/domain", 0700);
#endif

        return 0;
}