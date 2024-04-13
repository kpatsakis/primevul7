int unit_add_two_dependencies(Unit *u, UnitDependency d, UnitDependency e, Unit *other, bool add_reference) {
        int r;

        assert(u);

        if ((r = unit_add_dependency(u, d, other, add_reference)) < 0)
                return r;

        if ((r = unit_add_dependency(u, e, other, add_reference)) < 0)
                return r;

        return 0;
}