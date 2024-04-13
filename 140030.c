int seccomp_load_syscall_filter_set(uint32_t default_action, const SyscallFilterSet *set, uint32_t action) {
        uint32_t arch;
        int r;

        assert(set);

        /* The one-stop solution: allocate a seccomp object, add the specified filter to it, and apply it. Once for
         * earch local arch. */

        SECCOMP_FOREACH_LOCAL_ARCH(arch) {
                _cleanup_(seccomp_releasep) scmp_filter_ctx seccomp = NULL;

                log_debug("Operating on architecture: %s", seccomp_arch_to_string(arch));

                r = seccomp_init_for_arch(&seccomp, arch, default_action);
                if (r < 0)
                        return r;

                r = seccomp_add_syscall_filter_set(seccomp, set, action, NULL);
                if (r < 0) {
                        log_debug_errno(r, "Failed to add filter set, ignoring: %m");
                        continue;
                }

                r = seccomp_load(seccomp);
                if (IN_SET(r, -EPERM, -EACCES))
                        return r;
                if (r < 0)
                        log_debug_errno(r, "Failed to install filter set for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
        }

        return 0;
}