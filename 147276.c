static int manager_dispatch_sigchld(Manager *m) {
        assert(m);

        for (;;) {
                siginfo_t si = {};

                /* First we call waitd() for a PID and do not reap the
                 * zombie. That way we can still access /proc/$PID for
                 * it while it is a zombie. */
                if (waitid(P_ALL, 0, &si, WEXITED|WNOHANG|WNOWAIT) < 0) {

                        if (errno == ECHILD)
                                break;

                        if (errno == EINTR)
                                continue;

                        return -errno;
                }

                if (si.si_pid <= 0)
                        break;

                if (si.si_code == CLD_EXITED || si.si_code == CLD_KILLED || si.si_code == CLD_DUMPED) {
                        _cleanup_free_ char *name = NULL;
                        Unit *u;

                        get_process_comm(si.si_pid, &name);

                        log_debug("Child "PID_FMT" (%s) died (code=%s, status=%i/%s)",
                                  si.si_pid, strna(name),
                                  sigchld_code_to_string(si.si_code),
                                  si.si_status,
                                  strna(si.si_code == CLD_EXITED
                                        ? exit_status_to_string(si.si_status, EXIT_STATUS_FULL)
                                        : signal_to_string(si.si_status)));

                        /* And now figure out the unit this belongs
                         * to, it might be multiple... */
                        u = manager_get_unit_by_pid(m, si.si_pid);
                        if (u)
                                invoke_sigchld_event(m, u, &si);
                        u = hashmap_get(m->watch_pids1, LONG_TO_PTR(si.si_pid));
                        if (u)
                                invoke_sigchld_event(m, u, &si);
                        u = hashmap_get(m->watch_pids2, LONG_TO_PTR(si.si_pid));
                        if (u)
                                invoke_sigchld_event(m, u, &si);
                }

                /* And now, we actually reap the zombie. */
                if (waitid(P_PID, si.si_pid, &si, WEXITED) < 0) {
                        if (errno == EINTR)
                                continue;

                        return -errno;
                }
        }

        return 0;
}