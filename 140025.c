int seccomp_lock_personality(unsigned long personality) {
        uint32_t arch;
        int r;

        if (personality >= PERSONALITY_INVALID)
                return -EINVAL;

        SECCOMP_FOREACH_LOCAL_ARCH(arch) {
                _cleanup_(seccomp_releasep) scmp_filter_ctx seccomp = NULL;

                r = seccomp_init_for_arch(&seccomp, arch, SCMP_ACT_ALLOW);
                if (r < 0)
                        return r;

                r = seccomp_rule_add_exact(
                                seccomp,
                                SCMP_ACT_ERRNO(EPERM),
                                SCMP_SYS(personality),
                                1,
                                SCMP_A0(SCMP_CMP_NE, personality));
                if (r < 0) {
                        log_debug_errno(r, "Failed to add scheduler rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                        continue;
                }

                r = seccomp_load(seccomp);
                if (IN_SET(r, -EPERM, -EACCES))
                        return r;
                if (r < 0)
                        log_debug_errno(r, "Failed to enable personality lock for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
        }

        return 0;
}