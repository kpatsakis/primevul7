static int unit_add_mount_links(Unit *u) {
        char **i;
        int r;

        assert(u);

        STRV_FOREACH(i, u->requires_mounts_for) {
                char prefix[strlen(*i) + 1];

                PATH_FOREACH_PREFIX_MORE(prefix, *i) {
                        Unit *m;

                        r = manager_get_unit_by_path(u->manager, prefix, ".mount", &m);
                        if (r < 0)
                                return r;
                        if (r == 0)
                                continue;
                        if (m == u)
                                continue;

                        if (m->load_state != UNIT_LOADED)
                                continue;

                        r = unit_add_dependency(u, UNIT_AFTER, m, true);
                        if (r < 0)
                                return r;

                        if (m->fragment_path) {
                                r = unit_add_dependency(u, UNIT_REQUIRES, m, true);
                                if (r < 0)
                                        return r;
                        }
                }
        }

        return 0;
}