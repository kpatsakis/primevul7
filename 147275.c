static int manager_dispatch_idle_pipe_fd(sd_event_source *source, int fd, uint32_t revents, void *userdata) {
        Manager *m = userdata;

        assert(m);
        assert(m->idle_pipe[2] == fd);

        m->no_console_output = m->n_on_console > 0;

        m->idle_pipe_event_source = sd_event_source_unref(m->idle_pipe_event_source);
        manager_close_idle_pipe(m);

        return 0;
}