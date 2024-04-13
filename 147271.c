int unit_make_transient(Unit *u) {
        int r;

        assert(u);

        u->load_state = UNIT_STUB;
        u->load_error = 0;
        u->transient = true;

        free(u->fragment_path);
        u->fragment_path = NULL;

        if (u->manager->running_as == SYSTEMD_USER) {
                _cleanup_free_ char *c = NULL;

                r = user_config_home(&c);
                if (r < 0)
                        return r;
                if (r == 0)
                        return -ENOENT;

                u->fragment_path = strjoin(c, "/", u->id, NULL);
                if (!u->fragment_path)
                        return -ENOMEM;

                mkdir_p(c, 0755);
        } else {
                u->fragment_path = strappend("/run/systemd/system/", u->id);
                if (!u->fragment_path)
                        return -ENOMEM;

                mkdir_p("/run/systemd/system", 0755);
        }

        return write_string_file_atomic_label(u->fragment_path, "# Transient stub");
}