Unit *unit_new(Manager *m, size_t size) {
        Unit *u;

        assert(m);
        assert(size >= sizeof(Unit));

        u = malloc0(size);
        if (!u)
                return NULL;

        u->names = set_new(string_hash_func, string_compare_func);
        if (!u->names) {
                free(u);
                return NULL;
        }

        u->manager = m;
        u->type = _UNIT_TYPE_INVALID;
        u->deserialized_job = _JOB_TYPE_INVALID;
        u->default_dependencies = true;
        u->unit_file_state = _UNIT_FILE_STATE_INVALID;
        u->on_failure_job_mode = JOB_REPLACE;

        return u;
}