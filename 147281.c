bool unit_check_gc(Unit *u) {
        assert(u);

        if (u->load_state == UNIT_STUB)
                return true;

        if (UNIT_VTABLE(u)->no_gc)
                return true;

        if (u->no_gc)
                return true;

        if (u->job)
                return true;

        if (u->nop_job)
                return true;

        if (unit_active_state(u) != UNIT_INACTIVE)
                return true;

        if (u->refs)
                return true;

        if (UNIT_VTABLE(u)->check_gc)
                if (UNIT_VTABLE(u)->check_gc(u))
                        return true;

        return false;
}