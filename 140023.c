int seccomp_protect_sysctl(void) {
        uint32_t arch;
        int r;

        SECCOMP_FOREACH_LOCAL_ARCH(arch) {
                _cleanup_(seccomp_releasep) scmp_filter_ctx seccomp = NULL;

                log_debug("Operating on architecture: %s", seccomp_arch_to_string(arch));

                if (IN_SET(arch, SCMP_ARCH_X32, SCMP_ARCH_AARCH64))
                        /* No _sysctl syscall */
                        continue;

                r = seccomp_init_for_arch(&seccomp, arch, SCMP_ACT_ALLOW);
                if (r < 0)
                        return r;

                r = seccomp_rule_add_exact(
                                seccomp,
                                SCMP_ACT_ERRNO(EPERM),
                                SCMP_SYS(_sysctl),
                                0);
                if (r < 0) {
                        log_debug_errno(r, "Failed to add _sysctl() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                        continue;
                }

                r = seccomp_load(seccomp);
                if (IN_SET(r, -EPERM, -EACCES))
                        return r;
                if (r < 0)
                        log_debug_errno(r, "Failed to install sysctl protection rules for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
        }

        return 0;
}