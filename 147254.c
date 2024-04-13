void unit_tidy_watch_pids(Unit *u, pid_t except1, pid_t except2) {
        Iterator i;
        void *e;

        assert(u);

        /* Cleans dead PIDs from our list */

        SET_FOREACH(e, u->pids, i) {
                pid_t pid = PTR_TO_LONG(e);

                if (pid == except1 || pid == except2)
                        continue;

                if (kill(pid, 0) < 0 && errno == ESRCH)
                        set_remove(u->pids, e);
        }
}