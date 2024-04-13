static int manager_coldplug(Manager *m) {
        int r = 0, q;
        Iterator i;
        Unit *u;
        char *k;

        assert(m);

        /* Then, let's set up their initial state. */
        HASHMAP_FOREACH_KEY(u, k, m->units, i) {

                /* ignore aliases */
                if (u->id != k)
                        continue;

                if ((q = unit_coldplug(u)) < 0)
                        r = q;
        }

        return r;
}