static int manager_distribute_fds(Manager *m, FDSet *fds) {
        Unit *u;
        Iterator i;
        int r;

        assert(m);

        HASHMAP_FOREACH(u, m->units, i) {

                if (fdset_size(fds) <= 0)
                        break;

                if (UNIT_VTABLE(u)->distribute_fds) {
                        r = UNIT_VTABLE(u)->distribute_fds(u, fds);
                        if (r < 0)
                                return r;
                }
        }

        return 0;
}