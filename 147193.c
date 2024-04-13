static void manager_build_unit_path_cache(Manager *m) {
        char **i;
        _cleanup_free_ DIR *d = NULL;
        int r;

        assert(m);

        set_free_free(m->unit_path_cache);

        m->unit_path_cache = set_new(string_hash_func, string_compare_func);
        if (!m->unit_path_cache) {
                log_error("Failed to allocate unit path cache.");
                return;
        }

        /* This simply builds a list of files we know exist, so that
         * we don't always have to go to disk */

        STRV_FOREACH(i, m->lookup_paths.unit_path) {
                struct dirent *de;

                d = opendir(*i);
                if (!d) {
                        if (errno != ENOENT)
                                log_error("Failed to open directory %s: %m", *i);
                        continue;
                }

                while ((de = readdir(d))) {
                        char *p;

                        if (ignore_file(de->d_name))
                                continue;

                        p = strjoin(streq(*i, "/") ? "" : *i, "/", de->d_name, NULL);
                        if (!p) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        r = set_consume(m->unit_path_cache, p);
                        if (r < 0)
                                goto fail;
                }

                closedir(d);
                d = NULL;
        }

        return;

fail:
        log_error("Failed to build unit path cache: %s", strerror(-r));

        set_free_free(m->unit_path_cache);
        m->unit_path_cache = NULL;
}