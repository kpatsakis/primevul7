static int manager_connect_bus(Manager *m, bool reexecuting) {
        bool try_bus_connect;

        assert(m);

        try_bus_connect =
                m->kdbus_fd >= 0 ||
                reexecuting ||
                (m->running_as == SYSTEMD_USER && getenv("DBUS_SESSION_BUS_ADDRESS"));

        /* Try to connect to the busses, if possible. */
        return bus_init(m, try_bus_connect);
}