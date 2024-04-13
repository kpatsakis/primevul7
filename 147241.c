int unit_setup_exec_runtime(Unit *u) {
        ExecRuntime **rt;
        size_t offset;
        Iterator i;
        Unit *other;

        offset = UNIT_VTABLE(u)->exec_runtime_offset;
        assert(offset > 0);

        /* Check if ther already is an ExecRuntime for this unit? */
        rt = (ExecRuntime**) ((uint8_t*) u + offset);
        if (*rt)
                return 0;

        /* Try to get it from somebody else */
        SET_FOREACH(other, u->dependencies[UNIT_JOINS_NAMESPACE_OF], i) {

                *rt = unit_get_exec_runtime(other);
                if (*rt) {
                        exec_runtime_ref(*rt);
                        return 0;
                }
        }

        return exec_runtime_make(rt, unit_get_exec_context(u), u->id);
}