static int unit_add_default_dependencies(Unit *u) {

        static const UnitDependency deps[] = {
                UNIT_REQUIRED_BY,
                UNIT_REQUIRED_BY_OVERRIDABLE,
                UNIT_WANTED_BY,
                UNIT_BOUND_BY
        };

        Unit *target;
        Iterator i;
        int r;
        unsigned k;

        assert(u);

        for (k = 0; k < ELEMENTSOF(deps); k++)
                SET_FOREACH(target, u->dependencies[deps[k]], i) {
                        r = unit_add_default_target_dependency(u, target);
                        if (r < 0)
                                return r;
                }

        if (u->default_dependencies && unit_get_cgroup_context(u)) {
                if (UNIT_ISSET(u->slice))
                        r = unit_add_two_dependencies(u, UNIT_AFTER, UNIT_WANTS, UNIT_DEREF(u->slice), true);
                else
                        r = unit_add_two_dependencies_by_name(u, UNIT_AFTER, UNIT_WANTS, SPECIAL_ROOT_SLICE, NULL, true);

                if (r < 0)
                        return r;
        }

        return 0;
}