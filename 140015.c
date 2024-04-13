int seccomp_restrict_realtime(void) {
        static const int permitted_policies[] = {
                SCHED_OTHER,
                SCHED_BATCH,
                SCHED_IDLE,
        };

        int r, max_policy = 0;
        uint32_t arch;
        unsigned i;

        /* Determine the highest policy constant we want to allow */
        for (i = 0; i < ELEMENTSOF(permitted_policies); i++)
                if (permitted_policies[i] > max_policy)
                        max_policy = permitted_policies[i];

        SECCOMP_FOREACH_LOCAL_ARCH(arch) {
                _cleanup_(seccomp_releasep) scmp_filter_ctx seccomp = NULL;
                int p;

                log_debug("Operating on architecture: %s", seccomp_arch_to_string(arch));

                r = seccomp_init_for_arch(&seccomp, arch, SCMP_ACT_ALLOW);
                if (r < 0)
                        return r;

                /* Go through all policies with lower values than that, and block them -- unless they appear in the
                 * whitelist. */
                for (p = 0; p < max_policy; p++) {
                        bool good = false;

                        /* Check if this is in the whitelist. */
                        for (i = 0; i < ELEMENTSOF(permitted_policies); i++)
                                if (permitted_policies[i] == p) {
                                        good = true;
                                        break;
                                }

                        if (good)
                                continue;

                        /* Deny this policy */
                        r = seccomp_rule_add_exact(
                                        seccomp,
                                        SCMP_ACT_ERRNO(EPERM),
                                        SCMP_SYS(sched_setscheduler),
                                        1,
                                        SCMP_A1(SCMP_CMP_EQ, p));
                        if (r < 0) {
                                log_debug_errno(r, "Failed to add scheduler rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                                continue;
                        }
                }

                /* Blacklist all other policies, i.e. the ones with higher values. Note that all comparisons are
                 * unsigned here, hence no need no check for < 0 values. */
                r = seccomp_rule_add_exact(
                                seccomp,
                                SCMP_ACT_ERRNO(EPERM),
                                SCMP_SYS(sched_setscheduler),
                                1,
                                SCMP_A1(SCMP_CMP_GT, max_policy));
                if (r < 0) {
                        log_debug_errno(r, "Failed to add scheduler rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                        continue;
                }

                r = seccomp_load(seccomp);
                if (IN_SET(r, -EPERM, -EACCES))
                        return r;
                if (r < 0)
                        log_debug_errno(r, "Failed to install realtime protection rules for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
        }

        return 0;
}