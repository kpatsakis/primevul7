static int drop_in_file(Unit *u, UnitSetPropertiesMode mode, const char *name, char **_p, char **_q) {
        _cleanup_free_ char *b = NULL;
        char *p, *q;
        int r;

        assert(u);
        assert(name);
        assert(_p);
        assert(_q);
        assert(mode & (UNIT_PERSISTENT|UNIT_RUNTIME));

        b = xescape(name, "/.");
        if (!b)
                return -ENOMEM;

        if (!filename_is_safe(b))
                return -EINVAL;

        if (u->manager->running_as == SYSTEMD_USER) {
                _cleanup_free_ char *c = NULL;

                r = user_config_home(&c);
                if (r < 0)
                        return r;
                if (r == 0)
                        return -ENOENT;

                p = strjoin(c, "/", u->id, ".d", NULL);
        } else if (mode & UNIT_PERSISTENT)
                p = strjoin("/etc/systemd/system/", u->id, ".d", NULL);
        else
                p = strjoin("/run/systemd/system/", u->id, ".d", NULL);
        if (!p)
                return -ENOMEM;

        q = strjoin(p, "/90-", b, ".conf", NULL);
        if (!q) {
                free(p);
                return -ENOMEM;
        }

        *_p = p;
        *_q = q;
        return 0;
}