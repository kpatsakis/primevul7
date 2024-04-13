void manager_send_unit_plymouth(Manager *m, Unit *u) {
        int fd = -1;
        union sockaddr_union sa;
        int n = 0;
        char *message = NULL;

        /* Don't generate plymouth events if the service was already
         * started and we're just deserializing */
        if (m->n_reloading > 0)
                return;

        if (m->running_as != SYSTEMD_SYSTEM)
                return;

        if (detect_container(NULL) > 0)
                return;

        if (u->type != UNIT_SERVICE &&
            u->type != UNIT_MOUNT &&
            u->type != UNIT_SWAP)
                return;

        /* We set SOCK_NONBLOCK here so that we rather drop the
         * message then wait for plymouth */
        fd = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);
        if (fd < 0) {
                log_error("socket() failed: %m");
                return;
        }

        zero(sa);
        sa.sa.sa_family = AF_UNIX;
        strncpy(sa.un.sun_path+1, "/org/freedesktop/plymouthd", sizeof(sa.un.sun_path)-1);
        if (connect(fd, &sa.sa, offsetof(struct sockaddr_un, sun_path) + 1 + strlen(sa.un.sun_path+1)) < 0) {

                if (errno != EPIPE &&
                    errno != EAGAIN &&
                    errno != ENOENT &&
                    errno != ECONNREFUSED &&
                    errno != ECONNRESET &&
                    errno != ECONNABORTED)
                        log_error("connect() failed: %m");

                goto finish;
        }

        if (asprintf(&message, "U\002%c%s%n", (int) (strlen(u->id) + 1), u->id, &n) < 0) {
                log_oom();
                goto finish;
        }

        errno = 0;
        if (write(fd, message, n + 1) != n + 1) {

                if (errno != EPIPE &&
                    errno != EAGAIN &&
                    errno != ENOENT &&
                    errno != ECONNREFUSED &&
                    errno != ECONNRESET &&
                    errno != ECONNABORTED)
                        log_error("Failed to write Plymouth message: %m");

                goto finish;
        }

finish:
        if (fd >= 0)
                close_nointr_nofail(fd);

        free(message);
}