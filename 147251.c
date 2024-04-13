int unit_reload(Unit *u) {
        UnitActiveState state;
        Unit *following;

        assert(u);

        if (u->load_state != UNIT_LOADED)
                return -EINVAL;

        if (!unit_can_reload(u))
                return -EBADR;

        state = unit_active_state(u);
        if (state == UNIT_RELOADING)
                return -EALREADY;

        if (state != UNIT_ACTIVE) {
                log_warning_unit(u->id, "Unit %s cannot be reloaded because it is inactive.",
                                 u->id);
                return -ENOEXEC;
        }

        following = unit_following(u);
        if (following) {
                log_debug_unit(u->id, "Redirecting reload request from %s to %s.",
                               u->id, following->id);
                return unit_reload(following);
        }

        unit_status_log_starting_stopping_reloading(u, JOB_RELOAD);

        unit_add_to_dbus_queue(u);
        return UNIT_VTABLE(u)->reload(u);
}