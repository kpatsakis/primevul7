UnitFileState unit_get_unit_file_state(Unit *u) {
        assert(u);

        if (u->unit_file_state < 0 && u->fragment_path)
                u->unit_file_state = unit_file_get_state(
                                u->manager->running_as == SYSTEMD_SYSTEM ? UNIT_FILE_SYSTEM : UNIT_FILE_USER,
                                NULL, basename(u->fragment_path));

        return u->unit_file_state;
}