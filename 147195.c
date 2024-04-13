static int manager_setup_notify(Manager *m) {
        union {
                struct sockaddr sa;
                struct sockaddr_un un;
        } sa = {
                .sa.sa_family = AF_UNIX,
        };
        int one = 1, r;

        if (m->notify_fd < 0) {
                _cleanup_close_ int fd = -1;

                /* First free all secondary fields */
                free(m->notify_socket);
                m->notify_socket = NULL;
                m->notify_event_source = sd_event_source_unref(m->notify_event_source);

                fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);
                if (fd < 0) {
                        log_error("Failed to allocate notification socket: %m");
                        return -errno;
                }

                if (getpid() != 1 || detect_container(NULL) > 0)
                        snprintf(sa.un.sun_path, sizeof(sa.un.sun_path), NOTIFY_SOCKET "/%" PRIx64, random_u64());
                else
                        strncpy(sa.un.sun_path, NOTIFY_SOCKET, sizeof(sa.un.sun_path));
                sa.un.sun_path[0] = 0;

                r = bind(fd, &sa.sa, offsetof(struct sockaddr_un, sun_path) + 1 + strlen(sa.un.sun_path+1));
                if (r < 0) {
                        log_error("bind() failed: %m");
                        return -errno;
                }

                r = setsockopt(fd, SOL_SOCKET, SO_PASSCRED, &one, sizeof(one));
                if (r < 0) {
                        log_error("SO_PASSCRED failed: %m");
                        return -errno;
                }

                sa.un.sun_path[0] = '@';
                m->notify_socket = strdup(sa.un.sun_path);
                if (!m->notify_socket)
                        return log_oom();

                m->notify_fd = fd;
                fd = -1;

                log_debug("Using notification socket %s", m->notify_socket);
        }

        if (!m->notify_event_source) {
                r = sd_event_add_io(m->event, m->notify_fd, EPOLLIN, manager_dispatch_notify_fd, m, &m->notify_event_source);
                if (r < 0) {
                        log_error("Failed to allocate notify event source: %s", strerror(-r));
                        return -errno;
                }

                /* Process signals a bit earlier than SIGCHLD, so that we can
                 * still identify to which service an exit message belongs */
                r = sd_event_source_set_priority(m->notify_event_source, -7);
                if (r < 0) {
                        log_error("Failed to set priority of notify event source: %s", strerror(-r));
                        return r;
                }
        }

        return 0;
}