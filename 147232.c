int unit_serialize(Unit *u, FILE *f, FDSet *fds, bool serialize_jobs) {
        ExecRuntime *rt;
        int r;

        assert(u);
        assert(f);
        assert(fds);

        if (!unit_can_serialize(u))
                return 0;

        r = UNIT_VTABLE(u)->serialize(u, f, fds);
        if (r < 0)
                return r;

        rt = unit_get_exec_runtime(u);
        if (rt) {
                r = exec_runtime_serialize(rt, u, f, fds);
                if (r < 0)
                        return r;
        }

        dual_timestamp_serialize(f, "inactive-exit-timestamp", &u->inactive_exit_timestamp);
        dual_timestamp_serialize(f, "active-enter-timestamp", &u->active_enter_timestamp);
        dual_timestamp_serialize(f, "active-exit-timestamp", &u->active_exit_timestamp);
        dual_timestamp_serialize(f, "inactive-enter-timestamp", &u->inactive_enter_timestamp);
        dual_timestamp_serialize(f, "condition-timestamp", &u->condition_timestamp);

        if (dual_timestamp_is_set(&u->condition_timestamp))
                unit_serialize_item(u, f, "condition-result", yes_no(u->condition_result));

        unit_serialize_item(u, f, "transient", yes_no(u->transient));

        if (u->cgroup_path)
                unit_serialize_item(u, f, "cgroup", u->cgroup_path);

        if (serialize_jobs) {
                if (u->job) {
                        fprintf(f, "job\n");
                        job_serialize(u->job, f, fds);
                }

                if (u->nop_job) {
                        fprintf(f, "job\n");
                        job_serialize(u->nop_job, f, fds);
                }
        }

        /* End marker */
        fputc('\n', f);
        return 0;
}