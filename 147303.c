static bool manager_is_booting_or_shutting_down(Manager *m) {
        Unit *u;

        assert(m);

        /* Is the initial job still around? */
        if (manager_get_job(m, m->default_unit_job_id))
                return true;

        /* Is there a job for the shutdown target? */
        u = manager_get_unit(m, SPECIAL_SHUTDOWN_TARGET);
        if (u)
                return !!u->job;

        return false;
}