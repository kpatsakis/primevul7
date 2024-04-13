int seccomp_filter_set_add(Hashmap *filter, bool add, const SyscallFilterSet *set) {
        const char *i;
        int r;

        assert(set);

        NULSTR_FOREACH(i, set->value) {

                if (i[0] == '@') {
                        const SyscallFilterSet *more;

                        more = syscall_filter_set_find(i);
                        if (!more)
                                return -ENXIO;

                        r = seccomp_filter_set_add(filter, add, more);
                        if (r < 0)
                                return r;
                } else {
                        int id;

                        id = seccomp_syscall_resolve_name(i);
                        if (id == __NR_SCMP_ERROR) {
                                log_debug("Couldn't resolve system call, ignoring: %s", i);
                                continue;
                        }

                        if (add) {
                                r = hashmap_put(filter, INT_TO_PTR(id + 1), INT_TO_PTR(-1));
                                if (r < 0)
                                        return r;
                        } else
                                (void) hashmap_remove(filter, INT_TO_PTR(id + 1));
                }
        }

        return 0;
}