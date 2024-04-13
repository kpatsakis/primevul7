int manager_open_serialization(Manager *m, FILE **_f) {
        const char *path;
        int fd = -1;
        FILE *f;

        assert(_f);

        path = m->running_as == SYSTEMD_SYSTEM ? "/run/systemd" : "/tmp";
        fd = open_tmpfile(path, O_RDWR|O_CLOEXEC);
        if (fd < 0)
                return -errno;

        log_debug("Serializing state to %s", path);

        f = fdopen(fd, "w+");
        if (!f) {
                close_nointr_nofail(fd);
                return -errno;
        }

        *_f = f;

        return 0;
}