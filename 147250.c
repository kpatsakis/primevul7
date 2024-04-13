int unit_set_description(Unit *u, const char *description) {
        char *s;

        assert(u);

        if (isempty(description))
                s = NULL;
        else {
                s = strdup(description);
                if (!s)
                        return -ENOMEM;
        }

        free(u->description);
        u->description = s;

        unit_add_to_dbus_queue(u);
        return 0;
}