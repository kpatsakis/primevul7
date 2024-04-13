ExecContext *unit_get_exec_context(Unit *u) {
        size_t offset;
        assert(u);

        offset = UNIT_VTABLE(u)->exec_context_offset;
        if (offset <= 0)
                return NULL;

        return (ExecContext*) ((uint8_t*) u + offset);
}