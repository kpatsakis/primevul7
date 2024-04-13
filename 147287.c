int unit_coldplug(Unit *u) {
        int r;

        assert(u);

        if (UNIT_VTABLE(u)->coldplug)
                if ((r = UNIT_VTABLE(u)->coldplug(u)) < 0)
                        return r;

        if (u->job) {
                r = job_coldplug(u->job);
                if (r < 0)
                        return r;
        } else if (u->deserialized_job >= 0) {
                /* legacy */
                r = manager_add_job(u->manager, u->deserialized_job, u, JOB_IGNORE_REQUIREMENTS, false, NULL, NULL);
                if (r < 0)
                        return r;

                u->deserialized_job = _JOB_TYPE_INVALID;
        }

        return 0;
}