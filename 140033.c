int seccomp_add_syscall_filter_item(scmp_filter_ctx *seccomp, const char *name, uint32_t action, char **exclude) {
        int r;

        assert(seccomp);
        assert(name);

        if (strv_contains(exclude, name))
                return 0;

        if (name[0] == '@') {
                const SyscallFilterSet *other;

                other = syscall_filter_set_find(name);
                if (!other) {
                        log_debug("Filter set %s is not known!", name);
                        return -EINVAL;
                }

                r = seccomp_add_syscall_filter_set(seccomp, other, action, exclude);
                if (r < 0)
                        return r;
        } else {
                int id;

                id = seccomp_syscall_resolve_name(name);
                if (id == __NR_SCMP_ERROR) {
                        log_debug("System call %s is not known, ignoring.", name);
                        return 0;
                }

                r = seccomp_rule_add_exact(seccomp, action, id, 0);
                if (r < 0)
                        /* If the system call is not known on this architecture, then that's fine, let's ignore it */
                        log_debug_errno(r, "Failed to add rule for system call %s() / %d, ignoring: %m", name, id);
        }

        return 0;
}