static int watch_pids_in_path(Unit *u, const char *path) {
        _cleanup_closedir_ DIR *d = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        int ret = 0, r;

        assert(u);
        assert(path);

        /* Adds all PIDs from a specific cgroup path to the set of PIDs we watch. */

        r = cg_enumerate_processes(SYSTEMD_CGROUP_CONTROLLER, path, &f);
        if (r >= 0) {
                pid_t pid;

                while ((r = cg_read_pid(f, &pid)) > 0) {
                        r = unit_watch_pid(u, pid);
                        if (r < 0 && ret >= 0)
                                ret = r;
                }
                if (r < 0 && ret >= 0)
                        ret = r;

        } else if (ret >= 0)
                ret = r;

        r = cg_enumerate_subgroups(SYSTEMD_CGROUP_CONTROLLER, path, &d);
        if (r >= 0) {
                char *fn;

                while ((r = cg_read_subgroup(d, &fn)) > 0) {
                        _cleanup_free_ char *p = NULL;

                        p = strjoin(path, "/", fn, NULL);
                        free(fn);

                        if (!p)
                                return -ENOMEM;

                        r = watch_pids_in_path(u, p);
                        if (r < 0 && ret >= 0)
                                ret = r;
                }
                if (r < 0 && ret >= 0)
                        ret = r;

        } else if (ret >= 0)
                ret = r;

        return ret;
}