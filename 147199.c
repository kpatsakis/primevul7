int unit_remove_drop_in(Unit *u, UnitSetPropertiesMode mode, const char *name) {
        _cleanup_free_ char *p = NULL, *q = NULL;
        int r;

        assert(u);

        if (!(mode & (UNIT_PERSISTENT|UNIT_RUNTIME)))
                return 0;

        r = drop_in_file(u, mode, name, &p, &q);
        if (r < 0)
                return r;

        if (unlink(q) < 0)
                r = errno == ENOENT ? 0 : -errno;
        else
                r = 1;

        rmdir(p);
        return r;
}