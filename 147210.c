int unit_add_default_slice(Unit *u) {
        _cleanup_free_ char *b = NULL;
        const char *slice_name;
        Unit *slice;
        int r;

        assert(u);

        if (UNIT_ISSET(u->slice))
                return 0;

        if (!unit_get_cgroup_context(u))
                return 0;

        if (u->instance) {
                _cleanup_free_ char *prefix = NULL, *escaped = NULL;

                /* Implicitly place all instantiated units in their
                 * own per-template slice */

                prefix = unit_name_to_prefix(u->id);
                if (!prefix)
                        return -ENOMEM;

                /* The prefix is already escaped, but it might include
                 * "-" which has a special meaning for slice units,
                 * hence escape it here extra. */
                escaped = strreplace(prefix, "-", "\\x2d");
                if (!escaped)
                        return -ENOMEM;

                if (u->manager->running_as == SYSTEMD_SYSTEM)
                        b = strjoin("system-", escaped, ".slice", NULL);
                else
                        b = strappend(escaped, ".slice");
                if (!b)
                        return -ENOMEM;

                slice_name = b;
        } else
                slice_name =
                        u->manager->running_as == SYSTEMD_SYSTEM
                        ? SPECIAL_SYSTEM_SLICE
                        : SPECIAL_ROOT_SLICE;

        r = manager_load_unit(u->manager, slice_name, NULL, NULL, &slice);
        if (r < 0)
                return r;

        unit_ref_set(&u->slice, slice);
        return 0;
}