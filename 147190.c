static int create_generator_dir(Manager *m, char **generator, const char *name) {
        char *p;
        int r;

        assert(m);
        assert(generator);
        assert(name);

        if (*generator)
                return 0;

        if (m->running_as == SYSTEMD_SYSTEM && getpid() == 1) {
                /* systemd --system, not running --test */

                p = strappend("/run/systemd/", name);
                if (!p)
                        return log_oom();

                r = mkdir_p_label(p, 0755);
                if (r < 0) {
                        log_error("Failed to create generator directory %s: %s",
                                  p, strerror(-r));
                        free(p);
                        return r;
                }
        } else if (m->running_as == SYSTEMD_USER) {
                const char *s = NULL;

                s = getenv("XDG_RUNTIME_DIR");
                if (!s)
                        return -EINVAL;
                p = strjoin(s, "/systemd/", name, NULL);
                if (!p)
                        return log_oom();

                r = mkdir_p_label(p, 0755);
                if (r < 0) {
                        log_error("Failed to create generator directory %s: %s",
                                  p, strerror(-r));
                        free(p);
                        return r;
                }
        } else {
                /* systemd --system --test */

                p = strjoin("/tmp/systemd-", name, ".XXXXXX", NULL);
                if (!p)
                        return log_oom();

                if (!mkdtemp(p)) {
                        log_error("Failed to create generator directory %s: %m",
                                  p);
                        free(p);
                        return -errno;
                }
        }

        *generator = p;
        return 0;
}