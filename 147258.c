static Set *unit_pid_set(pid_t main_pid, pid_t control_pid) {
        Set *pid_set;
        int r;

        pid_set = set_new(trivial_hash_func, trivial_compare_func);
        if (!pid_set)
                return NULL;

        /* Exclude the main/control pids from being killed via the cgroup */
        if (main_pid > 0) {
                r = set_put(pid_set, LONG_TO_PTR(main_pid));
                if (r < 0)
                        goto fail;
        }

        if (control_pid > 0) {
                r = set_put(pid_set, LONG_TO_PTR(control_pid));
                if (r < 0)
                        goto fail;
        }

        return pid_set;

fail:
        set_free(pid_set);
        return NULL;
}