int set_unit_path(const char *p) {
        _cleanup_free_ char *c = NULL;

        /* This is mostly for debug purposes */
        c = path_make_absolute_cwd(p);
        if (setenv("SYSTEMD_UNIT_PATH", c, 0) < 0)
                return -errno;

        return 0;
}