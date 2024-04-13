void manager_undo_generators(Manager *m) {
        assert(m);

        remove_generator_dir(m, &m->generator_unit_path);
        remove_generator_dir(m, &m->generator_unit_path_early);
        remove_generator_dir(m, &m->generator_unit_path_late);
}