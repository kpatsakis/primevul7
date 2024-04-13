int unit_write_drop_in(Unit *u, UnitSetPropertiesMode mode, const char *name, const char *data) {
        _cleanup_free_ char *p = NULL, *q = NULL;
        int r;

        assert(u);
        assert(name);
        assert(data);

        if (!(mode & (UNIT_PERSISTENT|UNIT_RUNTIME)))
                return 0;

        r = drop_in_file(u, mode, name, &p, &q);
        if (r < 0)
                return r;

        mkdir_p(p, 0755);
        return write_string_file_atomic_label(q, data);
}