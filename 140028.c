int seccomp_restrict_namespaces(unsigned long retain) {
        uint32_t arch;
        int r;

        if (log_get_max_level() >= LOG_DEBUG) {
                _cleanup_free_ char *s = NULL;

                (void) namespace_flag_to_string_many(retain, &s);
                log_debug("Restricting namespace to: %s.", strna(s));
        }

        /* NOOP? */
        if ((retain & NAMESPACE_FLAGS_ALL) == NAMESPACE_FLAGS_ALL)
                return 0;

        SECCOMP_FOREACH_LOCAL_ARCH(arch) {
                _cleanup_(seccomp_releasep) scmp_filter_ctx seccomp = NULL;
                unsigned i;

                log_debug("Operating on architecture: %s", seccomp_arch_to_string(arch));

                r = seccomp_init_for_arch(&seccomp, arch, SCMP_ACT_ALLOW);
                if (r < 0)
                        return r;

                if ((retain & NAMESPACE_FLAGS_ALL) == 0)
                        /* If every single kind of namespace shall be prohibited, then let's block the whole setns() syscall
                         * altogether. */
                        r = seccomp_rule_add_exact(
                                        seccomp,
                                        SCMP_ACT_ERRNO(EPERM),
                                        SCMP_SYS(setns),
                                        0);
                else
                        /* Otherwise, block only the invocations with the appropriate flags in the loop below, but also the
                         * special invocation with a zero flags argument, right here. */
                        r = seccomp_rule_add_exact(
                                        seccomp,
                                        SCMP_ACT_ERRNO(EPERM),
                                        SCMP_SYS(setns),
                                        1,
                                        SCMP_A1(SCMP_CMP_EQ, 0));
                if (r < 0) {
                        log_debug_errno(r, "Failed to add setns() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                        continue;
                }

                for (i = 0; namespace_flag_map[i].name; i++) {
                        unsigned long f;

                        f = namespace_flag_map[i].flag;
                        if ((retain & f) == f) {
                                log_debug("Permitting %s.", namespace_flag_map[i].name);
                                continue;
                        }

                        log_debug("Blocking %s.", namespace_flag_map[i].name);

                        r = seccomp_rule_add_exact(
                                        seccomp,
                                        SCMP_ACT_ERRNO(EPERM),
                                        SCMP_SYS(unshare),
                                        1,
                                        SCMP_A0(SCMP_CMP_MASKED_EQ, f, f));
                        if (r < 0) {
                                log_debug_errno(r, "Failed to add unshare() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                                break;
                        }

                        /* On s390/s390x the first two parameters to clone are switched */
                        if (!IN_SET(arch, SCMP_ARCH_S390, SCMP_ARCH_S390X))
                                r = seccomp_rule_add_exact(
                                                seccomp,
                                                SCMP_ACT_ERRNO(EPERM),
                                                SCMP_SYS(clone),
                                                1,
                                                SCMP_A0(SCMP_CMP_MASKED_EQ, f, f));
                        else
                                r = seccomp_rule_add_exact(
                                                seccomp,
                                                SCMP_ACT_ERRNO(EPERM),
                                                SCMP_SYS(clone),
                                                1,
                                                SCMP_A1(SCMP_CMP_MASKED_EQ, f, f));
                        if (r < 0) {
                                log_debug_errno(r, "Failed to add clone() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                                break;
                        }

                        if ((retain & NAMESPACE_FLAGS_ALL) != 0) {
                                r = seccomp_rule_add_exact(
                                                seccomp,
                                                SCMP_ACT_ERRNO(EPERM),
                                                SCMP_SYS(setns),
                                                1,
                                                SCMP_A1(SCMP_CMP_MASKED_EQ, f, f));
                                if (r < 0) {
                                        log_debug_errno(r, "Failed to add setns() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                                        break;
                                }
                        }
                }
                if (r < 0)
                        continue;

                r = seccomp_load(seccomp);
                if (IN_SET(r, -EPERM, -EACCES))
                        return r;
                if (r < 0)
                        log_debug_errno(r, "Failed to install namespace restriction rules for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
        }

        return 0;
}