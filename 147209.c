int unit_add_dependency_by_name_inverse(Unit *u, UnitDependency d, const char *name, const char *path, bool add_reference) {
        Unit *other;
        int r;
        _cleanup_free_ char *s = NULL;

        assert(u);
        assert(name || path);

        if (!(name = resolve_template(u, name, path, &s)))
                return -ENOMEM;

        if ((r = manager_load_unit(u->manager, name, path, NULL, &other)) < 0)
                return r;

        r = unit_add_dependency(other, d, u, add_reference);

        return r;
}