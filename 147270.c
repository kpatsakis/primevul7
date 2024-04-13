static void unit_remove_transient(Unit *u) {
        char **i;

        assert(u);

        if (!u->transient)
                return;

        if (u->fragment_path)
                unlink(u->fragment_path);

        STRV_FOREACH(i, u->dropin_paths) {
                _cleanup_free_ char *p = NULL;
                int r;

                unlink(*i);

                r = path_get_parent(*i, &p);
                if (r >= 0)
                        rmdir(p);
        }
}