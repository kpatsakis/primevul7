int unit_watch_bus_name(Unit *u, const char *name) {
        assert(u);
        assert(name);

        /* Watch a specific name on the bus. We only support one unit
         * watching each name for now. */

        return hashmap_put(u->manager->watch_bus, name, u);
}