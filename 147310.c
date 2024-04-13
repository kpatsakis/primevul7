int unit_deserialize(Unit *u, FILE *f, FDSet *fds) {
        size_t offset;
        ExecRuntime **rt = NULL;
        int r;

        assert(u);
        assert(f);
        assert(fds);

        if (!unit_can_serialize(u))
                return 0;

        offset = UNIT_VTABLE(u)->exec_runtime_offset;
        if (offset > 0)
                rt = (ExecRuntime**) ((uint8_t*) u + offset);

        for (;;) {
                char line[LINE_MAX], *l, *v;
                size_t k;

                if (!fgets(line, sizeof(line), f)) {
                        if (feof(f))
                                return 0;
                        return -errno;
                }

                char_array_0(line);
                l = strstrip(line);

                /* End marker */
                if (l[0] == 0)
                        return 0;

                k = strcspn(l, "=");

                if (l[k] == '=') {
                        l[k] = 0;
                        v = l+k+1;
                } else
                        v = l+k;

                if (streq(l, "job")) {
                        if (v[0] == '\0') {
                                /* new-style serialized job */
                                Job *j = job_new_raw(u);
                                if (!j)
                                        return -ENOMEM;

                                r = job_deserialize(j, f, fds);
                                if (r < 0) {
                                        job_free(j);
                                        return r;
                                }

                                r = hashmap_put(u->manager->jobs, UINT32_TO_PTR(j->id), j);
                                if (r < 0) {
                                        job_free(j);
                                        return r;
                                }

                                r = job_install_deserialized(j);
                                if (r < 0) {
                                        hashmap_remove(u->manager->jobs, UINT32_TO_PTR(j->id));
                                        job_free(j);
                                        return r;
                                }

                                if (j->state == JOB_RUNNING)
                                        u->manager->n_running_jobs++;
                        } else {
                                /* legacy */
                                JobType type = job_type_from_string(v);
                                if (type < 0)
                                        log_debug("Failed to parse job type value %s", v);
                                else
                                        u->deserialized_job = type;
                        }
                        continue;
                } else if (streq(l, "inactive-exit-timestamp")) {
                        dual_timestamp_deserialize(v, &u->inactive_exit_timestamp);
                        continue;
                } else if (streq(l, "active-enter-timestamp")) {
                        dual_timestamp_deserialize(v, &u->active_enter_timestamp);
                        continue;
                } else if (streq(l, "active-exit-timestamp")) {
                        dual_timestamp_deserialize(v, &u->active_exit_timestamp);
                        continue;
                } else if (streq(l, "inactive-enter-timestamp")) {
                        dual_timestamp_deserialize(v, &u->inactive_enter_timestamp);
                        continue;
                } else if (streq(l, "condition-timestamp")) {
                        dual_timestamp_deserialize(v, &u->condition_timestamp);
                        continue;
                } else if (streq(l, "condition-result")) {
                        int b;

                        b = parse_boolean(v);
                        if (b < 0)
                                log_debug("Failed to parse condition result value %s", v);
                        else
                                u->condition_result = b;

                        continue;

                } else if (streq(l, "transient")) {
                        int b;

                        b = parse_boolean(v);
                        if (b < 0)
                                log_debug("Failed to parse transient bool %s", v);
                        else
                                u->transient = b;

                        continue;
                } else if (streq(l, "cgroup")) {
                        char *s;

                        s = strdup(v);
                        if (!s)
                                return -ENOMEM;

                        free(u->cgroup_path);
                        u->cgroup_path = s;

                        assert(hashmap_put(u->manager->cgroup_unit, s, u) == 1);
                        continue;
                }

                if (rt) {
                        r = exec_runtime_deserialize_item(rt, u, l, v, fds);
                        if (r < 0)
                                return r;
                        if (r > 0)
                                continue;
                }

                r = UNIT_VTABLE(u)->deserialize_item(u, l, v, fds);
                if (r < 0)
                        return r;
        }
}