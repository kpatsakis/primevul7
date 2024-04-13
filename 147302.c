int unit_watch_pid(Unit *u, pid_t pid) {
        int q, r;

        assert(u);
        assert(pid >= 1);

        /* Watch a specific PID. We only support one or two units
         * watching each PID for now, not more. */

        r = hashmap_ensure_allocated(&u->manager->watch_pids1, trivial_hash_func, trivial_compare_func);
        if (r < 0)
                return r;

        r = set_ensure_allocated(&u->pids, trivial_hash_func, trivial_compare_func);
        if (r < 0)
                return r;

        r = hashmap_put(u->manager->watch_pids1, LONG_TO_PTR(pid), u);
        if (r == -EEXIST) {
                r = hashmap_ensure_allocated(&u->manager->watch_pids2, trivial_hash_func, trivial_compare_func);
                if (r < 0)
                        return r;

                r = hashmap_put(u->manager->watch_pids2, LONG_TO_PTR(pid), u);
        }

        q = set_put(u->pids, LONG_TO_PTR(pid));
        if (q < 0)
                return q;

        return r;
}