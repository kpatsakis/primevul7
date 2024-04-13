void unit_notify(Unit *u, UnitActiveState os, UnitActiveState ns, bool reload_success) {
        Manager *m;
        bool unexpected;

        assert(u);
        assert(os < _UNIT_ACTIVE_STATE_MAX);
        assert(ns < _UNIT_ACTIVE_STATE_MAX);

        /* Note that this is called for all low-level state changes,
         * even if they might map to the same high-level
         * UnitActiveState! That means that ns == os is OK an expected
         * behavior here. For example: if a mount point is remounted
         * this function will be called too! */

        m = u->manager;

        if (m->n_reloading <= 0) {
                dual_timestamp ts;

                dual_timestamp_get(&ts);

                if (UNIT_IS_INACTIVE_OR_FAILED(os) && !UNIT_IS_INACTIVE_OR_FAILED(ns))
                        u->inactive_exit_timestamp = ts;
                else if (!UNIT_IS_INACTIVE_OR_FAILED(os) && UNIT_IS_INACTIVE_OR_FAILED(ns))
                        u->inactive_enter_timestamp = ts;

                if (!UNIT_IS_ACTIVE_OR_RELOADING(os) && UNIT_IS_ACTIVE_OR_RELOADING(ns))
                        u->active_enter_timestamp = ts;
                else if (UNIT_IS_ACTIVE_OR_RELOADING(os) && !UNIT_IS_ACTIVE_OR_RELOADING(ns))
                        u->active_exit_timestamp = ts;
        }

        if (UNIT_IS_INACTIVE_OR_FAILED(ns))
                unit_destroy_cgroup(u);

        /* Note that this doesn't apply to RemainAfterExit services exiting
         * sucessfully, since there's no change of state in that case. Which is
         * why it is handled in service_set_state() */
        if (UNIT_IS_INACTIVE_OR_FAILED(os) != UNIT_IS_INACTIVE_OR_FAILED(ns)) {
                ExecContext *ec = unit_get_exec_context(u);
                if (ec && exec_context_may_touch_console(ec)) {
                        if (UNIT_IS_INACTIVE_OR_FAILED(ns)) {
                                m->n_on_console --;

                                if (m->n_on_console == 0)
                                        /* unset no_console_output flag, since the console is free */
                                        m->no_console_output = false;
                        } else
                                m->n_on_console ++;
                }
        }

        if (u->job) {
                unexpected = false;

                if (u->job->state == JOB_WAITING)

                        /* So we reached a different state for this
                         * job. Let's see if we can run it now if it
                         * failed previously due to EAGAIN. */
                        job_add_to_run_queue(u->job);

                /* Let's check whether this state change constitutes a
                 * finished job, or maybe contradicts a running job and
                 * hence needs to invalidate jobs. */

                switch (u->job->type) {

                case JOB_START:
                case JOB_VERIFY_ACTIVE:

                        if (UNIT_IS_ACTIVE_OR_RELOADING(ns))
                                job_finish_and_invalidate(u->job, JOB_DONE, true);
                        else if (u->job->state == JOB_RUNNING && ns != UNIT_ACTIVATING) {
                                unexpected = true;

                                if (UNIT_IS_INACTIVE_OR_FAILED(ns))
                                        job_finish_and_invalidate(u->job, ns == UNIT_FAILED ? JOB_FAILED : JOB_DONE, true);
                        }

                        break;

                case JOB_RELOAD:
                case JOB_RELOAD_OR_START:

                        if (u->job->state == JOB_RUNNING) {
                                if (ns == UNIT_ACTIVE)
                                        job_finish_and_invalidate(u->job, reload_success ? JOB_DONE : JOB_FAILED, true);
                                else if (ns != UNIT_ACTIVATING && ns != UNIT_RELOADING) {
                                        unexpected = true;

                                        if (UNIT_IS_INACTIVE_OR_FAILED(ns))
                                                job_finish_and_invalidate(u->job, ns == UNIT_FAILED ? JOB_FAILED : JOB_DONE, true);
                                }
                        }

                        break;

                case JOB_STOP:
                case JOB_RESTART:
                case JOB_TRY_RESTART:

                        if (UNIT_IS_INACTIVE_OR_FAILED(ns))
                                job_finish_and_invalidate(u->job, JOB_DONE, true);
                        else if (u->job->state == JOB_RUNNING && ns != UNIT_DEACTIVATING) {
                                unexpected = true;
                                job_finish_and_invalidate(u->job, JOB_FAILED, true);
                        }

                        break;

                default:
                        assert_not_reached("Job type unknown");
                }

        } else
                unexpected = true;

        if (m->n_reloading <= 0) {

                /* If this state change happened without being
                 * requested by a job, then let's retroactively start
                 * or stop dependencies. We skip that step when
                 * deserializing, since we don't want to create any
                 * additional jobs just because something is already
                 * activated. */

                if (unexpected) {
                        if (UNIT_IS_INACTIVE_OR_FAILED(os) && UNIT_IS_ACTIVE_OR_ACTIVATING(ns))
                                retroactively_start_dependencies(u);
                        else if (UNIT_IS_ACTIVE_OR_ACTIVATING(os) && UNIT_IS_INACTIVE_OR_DEACTIVATING(ns))
                                retroactively_stop_dependencies(u);
                }

                /* stop unneeded units regardless if going down was expected or not */
                if (UNIT_IS_ACTIVE_OR_ACTIVATING(os) && UNIT_IS_INACTIVE_OR_DEACTIVATING(ns))
                        check_unneeded_dependencies(u);

                if (ns != os && ns == UNIT_FAILED) {
                        log_notice_unit(u->id,
                                        "Unit %s entered failed state.", u->id);
                        unit_start_on_failure(u);
                }
        }

        /* Some names are special */
        if (UNIT_IS_ACTIVE_OR_RELOADING(ns)) {

                if (unit_has_name(u, SPECIAL_DBUS_SERVICE))
                        /* The bus just might have become available,
                         * hence try to connect to it, if we aren't
                         * yet connected. */
                        bus_init(m, true);

                if (u->type == UNIT_SERVICE &&
                    !UNIT_IS_ACTIVE_OR_RELOADING(os) &&
                    m->n_reloading <= 0) {
                        /* Write audit record if we have just finished starting up */
                        manager_send_unit_audit(m, u, AUDIT_SERVICE_START, true);
                        u->in_audit = true;
                }

                if (!UNIT_IS_ACTIVE_OR_RELOADING(os))
                        manager_send_unit_plymouth(m, u);

        } else {

                /* We don't care about D-Bus here, since we'll get an
                 * asynchronous notification for it anyway. */

                if (u->type == UNIT_SERVICE &&
                    UNIT_IS_INACTIVE_OR_FAILED(ns) &&
                    !UNIT_IS_INACTIVE_OR_FAILED(os) &&
                    m->n_reloading <= 0) {

                        /* Hmm, if there was no start record written
                         * write it now, so that we always have a nice
                         * pair */
                        if (!u->in_audit) {
                                manager_send_unit_audit(m, u, AUDIT_SERVICE_START, ns == UNIT_INACTIVE);

                                if (ns == UNIT_INACTIVE)
                                        manager_send_unit_audit(m, u, AUDIT_SERVICE_STOP, true);
                        } else
                                /* Write audit record if we have just finished shutting down */
                                manager_send_unit_audit(m, u, AUDIT_SERVICE_STOP, ns == UNIT_INACTIVE);

                        u->in_audit = false;
                }
        }

        manager_recheck_journal(m);
        unit_trigger_notify(u);

        /* Maybe we finished startup and are now ready for being
         * stopped because unneeded? */
        if (u->manager->n_reloading <= 0)
                unit_check_unneeded(u);

        unit_add_to_dbus_queue(u);
        unit_add_to_gc_queue(u);
}