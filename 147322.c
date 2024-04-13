static void invoke_sigchld_event(Manager *m, Unit *u, siginfo_t *si) {
        assert(m);
        assert(u);
        assert(si);

        log_debug_unit(u->id, "Child "PID_FMT" belongs to %s", si->si_pid, u->id);

        unit_unwatch_pid(u, si->si_pid);
        UNIT_VTABLE(u)->sigchld_event(u, si->si_pid, si->si_code, si->si_status);
}