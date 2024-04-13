static void trim_generator_dir(Manager *m, char **generator) {
        assert(m);
        assert(generator);

        if (!*generator)
                return;

        if (rmdir(*generator) >= 0) {
                free(*generator);
                *generator = NULL;
        }

        return;
}