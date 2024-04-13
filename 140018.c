static int seccomp_add_syscall_filter_set(
                scmp_filter_ctx seccomp,
                const SyscallFilterSet *set,
                uint32_t action,
                char **exclude) {

        const char *sys;
        int r;

        assert(seccomp);
        assert(set);

        NULSTR_FOREACH(sys, set->value) {
                r = seccomp_add_syscall_filter_item(seccomp, sys, action, exclude);
                if (r < 0)
                        return r;
        }

        return 0;
}