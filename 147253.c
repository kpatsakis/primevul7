int unit_kill_common(
                Unit *u,
                KillWho who,
                int signo,
                pid_t main_pid,
                pid_t control_pid,
                sd_bus_error *error) {

        int r = 0;

        if (who == KILL_MAIN && main_pid <= 0) {
                if (main_pid < 0)
                        sd_bus_error_setf(error, BUS_ERROR_NO_SUCH_PROCESS, "%s units have no main processes", unit_type_to_string(u->type));
                else
                        sd_bus_error_set_const(error, BUS_ERROR_NO_SUCH_PROCESS, "No main process to kill");
                return -ESRCH;
        }

        if (who == KILL_CONTROL && control_pid <= 0) {
                if (control_pid < 0)
                        sd_bus_error_setf(error, BUS_ERROR_NO_SUCH_PROCESS, "%s units have no control processes", unit_type_to_string(u->type));
                else
                        sd_bus_error_set_const(error, BUS_ERROR_NO_SUCH_PROCESS, "No control process to kill");
                return -ESRCH;
        }

        if (who == KILL_CONTROL || who == KILL_ALL)
                if (control_pid > 0)
                        if (kill(control_pid, signo) < 0)
                                r = -errno;

        if (who == KILL_MAIN || who == KILL_ALL)
                if (main_pid > 0)
                        if (kill(main_pid, signo) < 0)
                                r = -errno;

        if (who == KILL_ALL && u->cgroup_path) {
                _cleanup_set_free_ Set *pid_set = NULL;
                int q;

                /* Exclude the main/control pids from being killed via the cgroup */
                pid_set = unit_pid_set(main_pid, control_pid);
                if (!pid_set)
                        return -ENOMEM;

                q = cg_kill_recursive(SYSTEMD_CGROUP_CONTROLLER, u->cgroup_path, signo, false, true, false, pid_set);
                if (q < 0 && q != -EAGAIN && q != -ESRCH && q != -ENOENT)
                        r = q;
        }

        return r;
}