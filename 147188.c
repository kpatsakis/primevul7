void unit_unwatch_pid(Unit *u, pid_t pid) {
        assert(u);
        assert(pid >= 1);

        hashmap_remove_value(u->manager->watch_pids1, LONG_TO_PTR(pid), u);
        hashmap_remove_value(u->manager->watch_pids2, LONG_TO_PTR(pid), u);
        set_remove(u->pids, LONG_TO_PTR(pid));
}