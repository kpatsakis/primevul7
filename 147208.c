int unit_kill_context(
                Unit *u,
                KillContext *c,
                bool sigkill,
                pid_t main_pid,
                pid_t control_pid,
                bool main_pid_alien) {

        int sig, wait_for_exit = false, r;

        assert(u);
        assert(c);

        if (c->kill_mode == KILL_NONE)
                return 0;

        sig = sigkill ? SIGKILL : c->kill_signal;

        if (main_pid > 0) {
                r = kill_and_sigcont(main_pid, sig);

                if (r < 0 && r != -ESRCH) {
                        _cleanup_free_ char *comm = NULL;
                        get_process_comm(main_pid, &comm);

                        log_warning_unit(u->id, "Failed to kill main process " PID_FMT " (%s): %s", main_pid, strna(comm), strerror(-r));
                } else {
                        if (!main_pid_alien)
                                wait_for_exit = true;

                        if (c->send_sighup && !sigkill)
                                kill(main_pid, SIGHUP);
                }
        }

        if (control_pid > 0) {
                r = kill_and_sigcont(control_pid, sig);

                if (r < 0 && r != -ESRCH) {
                        _cleanup_free_ char *comm = NULL;
                        get_process_comm(control_pid, &comm);

                        log_warning_unit(u->id, "Failed to kill control process " PID_FMT " (%s): %s", control_pid, strna(comm), strerror(-r));
                } else {
                        wait_for_exit = true;

                        if (c->send_sighup && !sigkill)
                                kill(control_pid, SIGHUP);
                }
        }

        if ((c->kill_mode == KILL_CONTROL_GROUP || (c->kill_mode == KILL_MIXED && sigkill)) && u->cgroup_path) {
                _cleanup_set_free_ Set *pid_set = NULL;

                /* Exclude the main/control pids from being killed via the cgroup */
                pid_set = unit_pid_set(main_pid, control_pid);
                if (!pid_set)
                        return -ENOMEM;

                r = cg_kill_recursive(SYSTEMD_CGROUP_CONTROLLER, u->cgroup_path, sig, true, true, false, pid_set);
                if (r < 0) {
                        if (r != -EAGAIN && r != -ESRCH && r != -ENOENT)
                                log_warning_unit(u->id, "Failed to kill control group: %s", strerror(-r));
                } else if (r > 0) {

                        wait_for_exit = true;

                        if (c->send_sighup && !sigkill) {
                                set_free(pid_set);

                                pid_set = unit_pid_set(main_pid, control_pid);
                                if (!pid_set)
                                        return -ENOMEM;

                                cg_kill_recursive(SYSTEMD_CGROUP_CONTROLLER, u->cgroup_path, SIGHUP, false, true, false, pid_set);
                        }
                }
        }

        return wait_for_exit;
}