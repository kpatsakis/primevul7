static void unit_free_requires_mounts_for(Unit *u) {
        char **j;

        STRV_FOREACH(j, u->requires_mounts_for) {
                char s[strlen(*j) + 1];

                PATH_FOREACH_PREFIX_MORE(s, *j) {
                        char *y;
                        Set *x;

                        x = hashmap_get2(u->manager->units_requiring_mounts_for, s, (void**) &y);
                        if (!x)
                                continue;

                        set_remove(x, u);

                        if (set_isempty(x)) {
                                hashmap_remove(u->manager->units_requiring_mounts_for, y);
                                free(y);
                                set_free(x);
                        }
                }
        }

        strv_free(u->requires_mounts_for);
        u->requires_mounts_for = NULL;
}