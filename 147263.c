bool unit_active_or_pending(Unit *u) {
        assert(u);

        /* Returns true if the unit is active or going up */

        if (UNIT_IS_ACTIVE_OR_ACTIVATING(unit_active_state(u)))
                return true;

        if (u->job &&
            (u->job->type == JOB_START ||
             u->job->type == JOB_RELOAD_OR_START ||
             u->job->type == JOB_RESTART))
                return true;

        return false;
}