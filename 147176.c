int unit_start(Unit *u) {
        UnitActiveState state;
        Unit *following;

        assert(u);

        if (u->load_state != UNIT_LOADED)
                return -EINVAL;

        /* If this is already started, then this will succeed. Note
         * that this will even succeed if this unit is not startable
         * by the user. This is relied on to detect when we need to
         * wait for units and when waiting is finished. */
        state = unit_active_state(u);
        if (UNIT_IS_ACTIVE_OR_RELOADING(state))
                return -EALREADY;

        /* If the conditions failed, don't do anything at all. If we
         * already are activating this call might still be useful to
         * speed up activation in case there is some hold-off time,
         * but we don't want to recheck the condition in that case. */
        if (state != UNIT_ACTIVATING &&
            !unit_condition_test(u)) {
                log_debug_unit(u->id, "Starting of %s requested but condition failed. Ignoring.", u->id);
                return -EALREADY;
        }

        /* Forward to the main object, if we aren't it. */
        following = unit_following(u);
        if (following) {
                log_debug_unit(u->id, "Redirecting start request from %s to %s.",
                               u->id, following->id);
                return unit_start(following);
        }

        unit_status_log_starting_stopping_reloading(u, JOB_START);
        unit_status_print_starting_stopping(u, JOB_START);

        /* If it is stopped, but we cannot start it, then fail */
        if (!UNIT_VTABLE(u)->start)
                return -EBADR;

        /* We don't suppress calls to ->start() here when we are
         * already starting, to allow this request to be used as a
         * "hurry up" call, for example when the unit is in some "auto
         * restart" state where it waits for a holdoff timer to elapse
         * before it will start again. */

        unit_add_to_dbus_queue(u);

        return UNIT_VTABLE(u)->start(u);
}