int unit_add_two_dependencies_by_name_inverse(Unit *u, UnitDependency d, UnitDependency e, const char *name, const char *path, bool add_reference) {
        Unit *other;
        int r;
        _cleanup_free_ char *s = NULL;

        assert(u);
        assert(name || path);

        if (!(name = resolve_template(u, name, path, &s)))
                return -ENOMEM;

        if ((r = manager_load_unit(u->manager, name, path, NULL, &other)) < 0)
                return r;

        if ((r = unit_add_two_dependencies(other, d, e, u, add_reference)) < 0)
                return r;

        return r;
}