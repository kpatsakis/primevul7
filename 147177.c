int unit_exec_context_defaults(Unit *u, ExecContext *c) {
        unsigned i;
        int r;

        assert(u);
        assert(c);

        /* This only copies in the ones that need memory */
        for (i = 0; i < RLIMIT_NLIMITS; i++)
                if (u->manager->rlimit[i] && !c->rlimit[i]) {
                        c->rlimit[i] = newdup(struct rlimit, u->manager->rlimit[i], 1);
                        if (!c->rlimit[i])
                                return -ENOMEM;
                }

        if (u->manager->running_as == SYSTEMD_USER &&
            !c->working_directory) {

                r = get_home_dir(&c->working_directory);
                if (r < 0)
                        return r;
        }

        return 0;
}