int unit_add_name(Unit *u, const char *text) {
        UnitType t;
        char *s, *i = NULL;
        int r;

        assert(u);
        assert(text);

        if (unit_name_is_template(text)) {
                if (!u->instance)
                        return -EINVAL;

                s = unit_name_replace_instance(text, u->instance);
        } else
                s = strdup(text);

        if (!s)
                return -ENOMEM;

        if (!unit_name_is_valid(s, TEMPLATE_INVALID)) {
                r = -EINVAL;
                goto fail;
        }

        assert_se((t = unit_name_to_type(s)) >= 0);

        if (u->type != _UNIT_TYPE_INVALID && t != u->type) {
                r = -EINVAL;
                goto fail;
        }

        r = unit_name_to_instance(s, &i);
        if (r < 0)
                goto fail;

        if (i && unit_vtable[t]->no_instances) {
                r = -EINVAL;
                goto fail;
        }

        /* Ensure that this unit is either instanced or not instanced,
         * but not both. */
        if (u->type != _UNIT_TYPE_INVALID && !u->instance != !i) {
                r = -EINVAL;
                goto fail;
        }

        if (unit_vtable[t]->no_alias &&
            !set_isempty(u->names) &&
            !set_get(u->names, s)) {
                r = -EEXIST;
                goto fail;
        }

        if (hashmap_size(u->manager->units) >= MANAGER_MAX_NAMES) {
                r = -E2BIG;
                goto fail;
        }

        r = set_put(u->names, s);
        if (r < 0) {
                if (r == -EEXIST)
                        r = 0;
                goto fail;
        }

        r = hashmap_put(u->manager->units, s, u);
        if (r < 0) {
                set_remove(u->names, s);
                goto fail;
        }

        if (u->type == _UNIT_TYPE_INVALID) {

                u->type = t;
                u->id = s;
                u->instance = i;

                LIST_PREPEND(units_by_type, u->manager->units_by_type[t], u);

                if (UNIT_VTABLE(u)->init)
                        UNIT_VTABLE(u)->init(u);
        } else
                free(i);

        unit_add_to_dbus_queue(u);
        return 0;

fail:
        free(s);
        free(i);

        return r;
}