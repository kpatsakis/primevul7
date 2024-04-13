static void manager_close_idle_pipe(Manager *m) {
        assert(m);

        close_pipe(m->idle_pipe);
        close_pipe(m->idle_pipe + 2);
}