CGroupContext *unit_get_cgroup_context(Unit *u) {
        size_t offset;

        offset = UNIT_VTABLE(u)->cgroup_context_offset;
        if (offset <= 0)
                return NULL;

        return (CGroupContext*) ((uint8_t*) u + offset);
}