KillContext *unit_get_kill_context(Unit *u) {
        size_t offset;
        assert(u);

        offset = UNIT_VTABLE(u)->kill_context_offset;
        if (offset <= 0)
                return NULL;

        return (KillContext*) ((uint8_t*) u + offset);
}