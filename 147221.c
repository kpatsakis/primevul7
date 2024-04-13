bool manager_is_reloading_or_reexecuting(Manager *m) {
        assert(m);

        return m->n_reloading != 0;
}