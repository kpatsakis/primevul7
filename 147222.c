void unit_unwatch_all_pids(Unit *u) {
        Iterator i;
        void *e;

        assert(u);

        SET_FOREACH(e, u->pids, i) {
                hashmap_remove_value(u->manager->watch_pids1, e, u);
                hashmap_remove_value(u->manager->watch_pids2, e, u);
        }

        set_free(u->pids);
        u->pids = NULL;
}