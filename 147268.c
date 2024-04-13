void unit_ref_unset(UnitRef *ref) {
        assert(ref);

        if (!ref->unit)
                return;

        LIST_REMOVE(refs, ref->unit->refs, ref);
        ref->unit = NULL;
}