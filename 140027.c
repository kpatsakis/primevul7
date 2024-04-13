int parse_syscall_archs(char **l, Set **archs) {
        _cleanup_set_free_ Set *_archs;
        char **s;
        int r;

        assert(l);
        assert(archs);

        r = set_ensure_allocated(&_archs, NULL);
        if (r < 0)
                return r;

        STRV_FOREACH(s, l) {
                uint32_t a;

                r = seccomp_arch_from_string(*s, &a);
                if (r < 0)
                        return -EINVAL;

                r = set_put(_archs, UINT32_TO_PTR(a + 1));
                if (r < 0)
                        return -ENOMEM;
        }

        *archs = _archs;
        _archs = NULL;

        return 0;
}