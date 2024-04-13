int unit_write_drop_in_private(Unit *u, UnitSetPropertiesMode mode, const char *name, const char *data) {
        _cleanup_free_ char *ndata = NULL;

        assert(u);
        assert(name);
        assert(data);

        if (!UNIT_VTABLE(u)->private_section)
                return -EINVAL;

        if (!(mode & (UNIT_PERSISTENT|UNIT_RUNTIME)))
                return 0;

        ndata = strjoin("[", UNIT_VTABLE(u)->private_section, "]\n", data, NULL);
        if (!ndata)
                return -ENOMEM;

        return unit_write_drop_in(u, mode, name, ndata);
}