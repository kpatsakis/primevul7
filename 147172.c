void manager_run_generators(Manager *m) {
        _cleanup_closedir_ DIR *d = NULL;
        const char *generator_path;
        const char *argv[5];
        int r;

        assert(m);

        generator_path = m->running_as == SYSTEMD_SYSTEM ? SYSTEM_GENERATOR_PATH : USER_GENERATOR_PATH;
        d = opendir(generator_path);
        if (!d) {
                if (errno == ENOENT)
                        return;

                log_error("Failed to enumerate generator directory %s: %m",
                          generator_path);
                return;
        }

        r = create_generator_dir(m, &m->generator_unit_path, "generator");
        if (r < 0)
                goto finish;

        r = create_generator_dir(m, &m->generator_unit_path_early, "generator.early");
        if (r < 0)
                goto finish;

        r = create_generator_dir(m, &m->generator_unit_path_late, "generator.late");
        if (r < 0)
                goto finish;

        argv[0] = NULL; /* Leave this empty, execute_directory() will fill something in */
        argv[1] = m->generator_unit_path;
        argv[2] = m->generator_unit_path_early;
        argv[3] = m->generator_unit_path_late;
        argv[4] = NULL;

        RUN_WITH_UMASK(0022)
                execute_directory(generator_path, d, (char**) argv);

finish:
        trim_generator_dir(m, &m->generator_unit_path);
        trim_generator_dir(m, &m->generator_unit_path_early);
        trim_generator_dir(m, &m->generator_unit_path_late);
}