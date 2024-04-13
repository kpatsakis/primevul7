void unit_start_on_failure(Unit *u) {
        Unit *other;
        Iterator i;

        assert(u);

        if (set_size(u->dependencies[UNIT_ON_FAILURE]) <= 0)
                return;

        log_info_unit(u->id, "Triggering OnFailure= dependencies of %s.", u->id);

        SET_FOREACH(other, u->dependencies[UNIT_ON_FAILURE], i) {
                int r;

                r = manager_add_job(u->manager, JOB_START, other, u->on_failure_job_mode, true, NULL, NULL);
                if (r < 0)
                        log_error_unit(u->id, "Failed to enqueue OnFailure= job: %s", strerror(-r));
        }
}