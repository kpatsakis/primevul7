int unit_stop(Unit *u) {
        UnitActiveState state;
        Unit *following;

        assert(u);

        state = unit_active_state(u);
        if (UNIT_IS_INACTIVE_OR_FAILED(state))
                return -EALREADY;

        if ((following = unit_following(u))) {
                log_debug_unit(u->id, "Redirecting stop request from %s to %s.",
                               u->id, following->id);
                return unit_stop(following);
        }

        unit_status_log_starting_stopping_reloading(u, JOB_STOP);
        unit_status_print_starting_stopping(u, JOB_STOP);

        if (!UNIT_VTABLE(u)->stop)
                return -EBADR;

        unit_add_to_dbus_queue(u);

        return UNIT_VTABLE(u)->stop(u);
}