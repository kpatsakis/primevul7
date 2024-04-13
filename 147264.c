int unit_choose_id(Unit *u, const char *name) {
        char *s, *i;
        _cleanup_free_ char *t = NULL;
        int r;

        assert(u);
        assert(name);

        if (unit_name_is_template(name)) {

                if (!u->instance)
                        return -EINVAL;

                t = unit_name_replace_instance(name, u->instance);
                if (!t)
                        return -ENOMEM;

                name = t;
        }

        /* Selects one of the names of this unit as the id */
        s = set_get(u->names, (char*) name);

        if (!s)
                return -ENOENT;

        r = unit_name_to_instance(s, &i);
        if (r < 0)
                return r;

        u->id = s;

        free(u->instance);
        u->instance = i;

        unit_add_to_dbus_queue(u);

        return 0;
}