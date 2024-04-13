int manager_serialize(Manager *m, FILE *f, FDSet *fds, bool switching_root) {
        Iterator i;
        Unit *u;
        const char *t;
        char **e;
        int r;

        assert(m);
        assert(f);
        assert(fds);

        m->n_reloading ++;

        fprintf(f, "current-job-id=%i\n", m->current_job_id);
        fprintf(f, "taint-usr=%s\n", yes_no(m->taint_usr));
        fprintf(f, "n-installed-jobs=%u\n", m->n_installed_jobs);
        fprintf(f, "n-failed-jobs=%u\n", m->n_failed_jobs);

        dual_timestamp_serialize(f, "firmware-timestamp", &m->firmware_timestamp);
        dual_timestamp_serialize(f, "loader-timestamp", &m->loader_timestamp);
        dual_timestamp_serialize(f, "kernel-timestamp", &m->kernel_timestamp);
        dual_timestamp_serialize(f, "initrd-timestamp", &m->initrd_timestamp);

        if (!in_initrd()) {
                dual_timestamp_serialize(f, "userspace-timestamp", &m->userspace_timestamp);
                dual_timestamp_serialize(f, "finish-timestamp", &m->finish_timestamp);
                dual_timestamp_serialize(f, "security-start-timestamp", &m->security_start_timestamp);
                dual_timestamp_serialize(f, "security-finish-timestamp", &m->security_finish_timestamp);
                dual_timestamp_serialize(f, "generators-start-timestamp", &m->generators_start_timestamp);
                dual_timestamp_serialize(f, "generators-finish-timestamp", &m->generators_finish_timestamp);
                dual_timestamp_serialize(f, "units-load-start-timestamp", &m->units_load_start_timestamp);
                dual_timestamp_serialize(f, "units-load-finish-timestamp", &m->units_load_finish_timestamp);
        }

        if (!switching_root) {
                STRV_FOREACH(e, m->environment) {
                        _cleanup_free_ char *ce;

                        ce = cescape(*e);
                        if (!ce)
                                return -ENOMEM;

                        fprintf(f, "env=%s\n", *e);
                }
        }

        if (m->notify_fd >= 0) {
                int copy;

                copy = fdset_put_dup(fds, m->notify_fd);
                if (copy < 0)
                        return copy;

                fprintf(f, "notify-fd=%i\n", copy);
                fprintf(f, "notify-socket=%s\n", m->notify_socket);
        }

        if (m->kdbus_fd >= 0) {
                int copy;

                copy = fdset_put_dup(fds, m->kdbus_fd);
                if (copy < 0)
                        return copy;

                fprintf(f, "kdbus-fd=%i\n", copy);
        }

        bus_serialize(m, f);

        fputc('\n', f);

        HASHMAP_FOREACH_KEY(u, t, m->units, i) {
                if (u->id != t)
                        continue;

                if (!unit_can_serialize(u))
                        continue;

                /* Start marker */
                fputs(u->id, f);
                fputc('\n', f);

                r = unit_serialize(u, f, fds, !switching_root);
                if (r < 0) {
                        m->n_reloading --;
                        return r;
                }
        }

        assert(m->n_reloading > 0);
        m->n_reloading --;

        if (ferror(f))
                return -EIO;

        r = bus_fdset_add_all(m, fds);
        if (r < 0)
                return r;

        return 0;
}