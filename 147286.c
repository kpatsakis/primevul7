Unit* unit_ref_set(UnitRef *ref, Unit *u) {
        assert(ref);
        assert(u);

        if (ref->unit)
                unit_ref_unset(ref);

        ref->unit = u;
        LIST_PREPEND(refs, u->refs, ref);
        return u;
}