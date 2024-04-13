int unit_kill(Unit *u, KillWho w, int signo, sd_bus_error *error) {
        assert(u);
        assert(w >= 0 && w < _KILL_WHO_MAX);
        assert(signo > 0);
        assert(signo < _NSIG);

        if (!UNIT_VTABLE(u)->kill)
                return -ENOTSUP;

        return UNIT_VTABLE(u)->kill(u, w, signo, error);
}