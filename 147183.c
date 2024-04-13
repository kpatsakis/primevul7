int unit_add_node_link(Unit *u, const char *what, bool wants) {
        Unit *device;
        _cleanup_free_ char *e = NULL;
        int r;

        assert(u);

        if (!what)
                return 0;

        /* Adds in links to the device node that this unit is based on */

        if (!is_device_path(what))
                return 0;

        e = unit_name_from_path(what, ".device");
        if (!e)
                return -ENOMEM;

        r = manager_load_unit(u->manager, e, NULL, NULL, &device);

        if (r < 0)
                return r;

        r = unit_add_two_dependencies(u, UNIT_AFTER, UNIT_BINDS_TO, device, true);
        if (r < 0)
                return r;

        if (wants) {
                r = unit_add_dependency(device, UNIT_WANTS, u, false);
                if (r < 0)
                        return r;
        }

        return 0;
}