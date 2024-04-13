int unit_require_mounts_for(Unit *u, const char *path) {
        char prefix[strlen(path) + 1], *p;
        int r;

        assert(u);
        assert(path);

        /* Registers a unit for requiring a certain path and all its
         * prefixes. We keep a simple array of these paths in the
         * unit, since its usually short. However, we build a prefix
         * table for all possible prefixes so that new appearing mount
         * units can easily determine which units to make themselves a
         * dependency of. */

        if (!path_is_absolute(path))
                return -EINVAL;

        p = strdup(path);
        if (!p)
                return -ENOMEM;

        path_kill_slashes(p);

        if (!path_is_safe(p)) {
                free(p);
                return -EPERM;
        }

        if (strv_contains(u->requires_mounts_for, p)) {
                free(p);
                return 0;
        }

        r = strv_push(&u->requires_mounts_for, p);
        if (r < 0) {
                free(p);
                return r;
        }

        PATH_FOREACH_PREFIX_MORE(prefix, p) {
                Set *x;

                x = hashmap_get(u->manager->units_requiring_mounts_for, prefix);
                if (!x) {
                        char *q;

                        if (!u->manager->units_requiring_mounts_for) {
                                u->manager->units_requiring_mounts_for = hashmap_new(string_hash_func, string_compare_func);
                                if (!u->manager->units_requiring_mounts_for)
                                        return -ENOMEM;
                        }

                        q = strdup(prefix);
                        if (!q)
                                return -ENOMEM;

                        x = set_new(NULL, NULL);
                        if (!x) {
                                free(q);
                                return -ENOMEM;
                        }

                        r = hashmap_put(u->manager->units_requiring_mounts_for, q, x);
                        if (r < 0) {
                                free(q);
                                set_free(x);
                                return r;
                        }
                }

                r = set_put(x, u);
                if (r < 0)
                        return r;
        }

        return 0;
}