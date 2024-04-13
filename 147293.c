int unit_add_dependency(Unit *u, UnitDependency d, Unit *other, bool add_reference) {

        static const UnitDependency inverse_table[_UNIT_DEPENDENCY_MAX] = {
                [UNIT_REQUIRES] = UNIT_REQUIRED_BY,
                [UNIT_REQUIRES_OVERRIDABLE] = UNIT_REQUIRED_BY_OVERRIDABLE,
                [UNIT_WANTS] = UNIT_WANTED_BY,
                [UNIT_REQUISITE] = UNIT_REQUIRED_BY,
                [UNIT_REQUISITE_OVERRIDABLE] = UNIT_REQUIRED_BY_OVERRIDABLE,
                [UNIT_BINDS_TO] = UNIT_BOUND_BY,
                [UNIT_PART_OF] = UNIT_CONSISTS_OF,
                [UNIT_REQUIRED_BY] = _UNIT_DEPENDENCY_INVALID,
                [UNIT_REQUIRED_BY_OVERRIDABLE] = _UNIT_DEPENDENCY_INVALID,
                [UNIT_WANTED_BY] = _UNIT_DEPENDENCY_INVALID,
                [UNIT_BOUND_BY] = UNIT_BINDS_TO,
                [UNIT_CONSISTS_OF] = UNIT_PART_OF,
                [UNIT_CONFLICTS] = UNIT_CONFLICTED_BY,
                [UNIT_CONFLICTED_BY] = UNIT_CONFLICTS,
                [UNIT_BEFORE] = UNIT_AFTER,
                [UNIT_AFTER] = UNIT_BEFORE,
                [UNIT_ON_FAILURE] = _UNIT_DEPENDENCY_INVALID,
                [UNIT_REFERENCES] = UNIT_REFERENCED_BY,
                [UNIT_REFERENCED_BY] = UNIT_REFERENCES,
                [UNIT_TRIGGERS] = UNIT_TRIGGERED_BY,
                [UNIT_TRIGGERED_BY] = UNIT_TRIGGERS,
                [UNIT_PROPAGATES_RELOAD_TO] = UNIT_RELOAD_PROPAGATED_FROM,
                [UNIT_RELOAD_PROPAGATED_FROM] = UNIT_PROPAGATES_RELOAD_TO,
                [UNIT_JOINS_NAMESPACE_OF] = UNIT_JOINS_NAMESPACE_OF,
        };
        int r, q = 0, v = 0, w = 0;

        assert(u);
        assert(d >= 0 && d < _UNIT_DEPENDENCY_MAX);
        assert(other);

        u = unit_follow_merge(u);
        other = unit_follow_merge(other);

        /* We won't allow dependencies on ourselves. We will not
         * consider them an error however. */
        if (u == other)
                return 0;

        r = set_ensure_allocated(&u->dependencies[d], trivial_hash_func, trivial_compare_func);
        if (r < 0)
                return r;

        if (inverse_table[d] != _UNIT_DEPENDENCY_INVALID) {
                r = set_ensure_allocated(&other->dependencies[inverse_table[d]], trivial_hash_func, trivial_compare_func);
                if (r < 0)
                        return r;
        }

        if (add_reference) {
                r = set_ensure_allocated(&u->dependencies[UNIT_REFERENCES], trivial_hash_func, trivial_compare_func);
                if (r < 0)
                        return r;

                r = set_ensure_allocated(&other->dependencies[UNIT_REFERENCED_BY], trivial_hash_func, trivial_compare_func);
                if (r < 0)
                        return r;
        }

        q = set_put(u->dependencies[d], other);
        if (q < 0)
                return q;

        if (inverse_table[d] != _UNIT_DEPENDENCY_INVALID && inverse_table[d] != d) {
                v = set_put(other->dependencies[inverse_table[d]], u);
                if (v < 0) {
                        r = v;
                        goto fail;
                }
        }

        if (add_reference) {
                w = set_put(u->dependencies[UNIT_REFERENCES], other);
                if (w < 0) {
                        r = w;
                        goto fail;
                }

                r = set_put(other->dependencies[UNIT_REFERENCED_BY], u);
                if (r < 0)
                        goto fail;
        }

        unit_add_to_dbus_queue(u);
        return 0;

fail:
        if (q > 0)
                set_remove(u->dependencies[d], other);

        if (v > 0)
                set_remove(other->dependencies[inverse_table[d]], u);

        if (w > 0)
                set_remove(u->dependencies[UNIT_REFERENCES], other);

        return r;
}