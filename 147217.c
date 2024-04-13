int manager_enumerate(Manager *m) {
        int r = 0, q;
        UnitType c;

        assert(m);

        /* Let's ask every type to load all units from disk/kernel
         * that it might know */
        for (c = 0; c < _UNIT_TYPE_MAX; c++)
                if (unit_vtable[c]->enumerate) {
                        q = unit_vtable[c]->enumerate(m);
                        if (q < 0)
                                r = q;
                }

        manager_dispatch_load_queue(m);
        return r;
}