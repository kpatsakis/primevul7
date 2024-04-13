bool unit_need_daemon_reload(Unit *u) {
        _cleanup_strv_free_ char **t = NULL;
        char **path;
        struct stat st;
        unsigned loaded_cnt, current_cnt;

        assert(u);

        if (u->fragment_path) {
                zero(st);
                if (stat(u->fragment_path, &st) < 0)
                        /* What, cannot access this anymore? */
                        return true;

                if (u->fragment_mtime > 0 &&
                    timespec_load(&st.st_mtim) != u->fragment_mtime)
                        return true;
        }

        if (u->source_path) {
                zero(st);
                if (stat(u->source_path, &st) < 0)
                        return true;

                if (u->source_mtime > 0 &&
                    timespec_load(&st.st_mtim) != u->source_mtime)
                        return true;
        }

        t = unit_find_dropin_paths(u);
        loaded_cnt = strv_length(t);
        current_cnt = strv_length(u->dropin_paths);

        if (loaded_cnt == current_cnt) {
                if (loaded_cnt == 0)
                        return false;

                if (strv_overlap(u->dropin_paths, t)) {
                        STRV_FOREACH(path, u->dropin_paths) {
                                zero(st);
                                if (stat(*path, &st) < 0)
                                        return true;

                                if (u->dropin_mtime > 0 &&
                                    timespec_load(&st.st_mtim) > u->dropin_mtime)
                                        return true;
                        }

                        return false;
                } else
                        return true;
        } else
                return true;
}