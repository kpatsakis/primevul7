static void remove_generator_dir(Manager *m, char **generator) {
        assert(m);
        assert(generator);

        if (!*generator)
                return;

        strv_remove(m->lookup_paths.unit_path, *generator);
        rm_rf(*generator, false, true, false);

        free(*generator);
        *generator = NULL;
}