static int manager_watch_idle_pipe(Manager *m) {
        int r;

        assert(m);

        if (m->idle_pipe_event_source)
                return 0;

        if (m->idle_pipe[2] < 0)
                return 0;

        r = sd_event_add_io(m->event, m->idle_pipe[2], EPOLLIN, manager_dispatch_idle_pipe_fd, m, &m->idle_pipe_event_source);
        if (r < 0) {
                log_error("Failed to watch idle pipe: %s", strerror(-r));
                return r;
        }

        return 0;
}