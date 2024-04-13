static void merge_dependencies(Unit *u, Unit *other, UnitDependency d) {
        Iterator i;
        Unit *back;
        int r;

        assert(u);
        assert(other);
        assert(d < _UNIT_DEPENDENCY_MAX);

        /* Fix backwards pointers */
        SET_FOREACH(back, other->dependencies[d], i) {
                UnitDependency k;

                for (k = 0; k < _UNIT_DEPENDENCY_MAX; k++) {
                        r = set_remove_and_put(back->dependencies[k], other, u);
                        if (r == -EEXIST)
                                set_remove(back->dependencies[k], other);
                        else
                                assert(r >= 0 || r == -ENOENT);
                }
        }

        complete_move(&u->dependencies[d], &other->dependencies[d]);

        set_free(other->dependencies[d]);
        other->dependencies[d] = NULL;
}