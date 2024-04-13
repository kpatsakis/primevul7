int seccomp_load_syscall_filter_set_raw(uint32_t default_action, Hashmap* set, uint32_t action) {
        uint32_t arch;
        int r;

        /* Similar to seccomp_load_syscall_filter_set(), but takes a raw Set* of syscalls, instead of a
         * SyscallFilterSet* table. */

        if (hashmap_isempty(set) && default_action == SCMP_ACT_ALLOW)
                return 0;

        SECCOMP_FOREACH_LOCAL_ARCH(arch) {
                _cleanup_(seccomp_releasep) scmp_filter_ctx seccomp = NULL;
                Iterator i;
                void *id, *val;

                log_debug("Operating on architecture: %s", seccomp_arch_to_string(arch));

                r = seccomp_init_for_arch(&seccomp, arch, default_action);
                if (r < 0)
                        return r;

                HASHMAP_FOREACH_KEY(val, id, set, i) {
                        uint32_t a = action;
                        int e = PTR_TO_INT(val);

                        if (action != SCMP_ACT_ALLOW && e >= 0)
                                a = SCMP_ACT_ERRNO(e);

                        r = seccomp_rule_add_exact(seccomp, a, PTR_TO_INT(id) - 1, 0);
                        if (r < 0) {
                                /* If the system call is not known on this architecture, then that's fine, let's ignore it */
                                _cleanup_free_ char *n = NULL;

                                n = seccomp_syscall_resolve_num_arch(arch, PTR_TO_INT(id) - 1);
                                log_debug_errno(r, "Failed to add rule for system call %s() / %d, ignoring: %m", strna(n), PTR_TO_INT(id) - 1);
                        }
                }

                r = seccomp_load(seccomp);
                if (IN_SET(r, -EPERM, -EACCES))
                        return r;
                if (r < 0)
                        log_debug_errno(r, "Failed to install filter set for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
        }

        return 0;
}