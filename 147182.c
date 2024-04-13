ExecRuntime *unit_get_exec_runtime(Unit *u) {
        size_t offset;

        offset = UNIT_VTABLE(u)->exec_runtime_offset;
        if (offset <= 0)
                return NULL;

        return *(ExecRuntime**) ((uint8_t*) u + offset);
}