int seccomp_restrict_address_families(Set *address_families, bool whitelist) {
        uint32_t arch;
        int r;

        SECCOMP_FOREACH_LOCAL_ARCH(arch) {
                _cleanup_(seccomp_releasep) scmp_filter_ctx seccomp = NULL;
                bool supported;
                Iterator i;

                log_debug("Operating on architecture: %s", seccomp_arch_to_string(arch));

                switch (arch) {

                case SCMP_ARCH_X86_64:
                case SCMP_ARCH_X32:
                case SCMP_ARCH_ARM:
                case SCMP_ARCH_AARCH64:
                case SCMP_ARCH_PPC64:
                case SCMP_ARCH_PPC64LE:
                        /* These we know we support (i.e. are the ones that do not use socketcall()) */
                        supported = true;
                        break;

                case SCMP_ARCH_S390:
                case SCMP_ARCH_S390X:
                case SCMP_ARCH_PPC:
                case SCMP_ARCH_X86:
                default:
                        /* These we either know we don't support (i.e. are the ones that do use socketcall()), or we
                         * don't know */
                        supported = false;
                        break;
                }

                if (!supported)
                        continue;

                r = seccomp_init_for_arch(&seccomp, arch, SCMP_ACT_ALLOW);
                if (r < 0)
                        return r;

                if (whitelist) {
                        int af, first = 0, last = 0;
                        void *afp;

                        /* If this is a whitelist, we first block the address families that are out of range and then
                         * everything that is not in the set. First, we find the lowest and highest address family in
                         * the set. */

                        SET_FOREACH(afp, address_families, i) {
                                af = PTR_TO_INT(afp);

                                if (af <= 0 || af >= af_max())
                                        continue;

                                if (first == 0 || af < first)
                                        first = af;

                                if (last == 0 || af > last)
                                        last = af;
                        }

                        assert((first == 0) == (last == 0));

                        if (first == 0) {

                                /* No entries in the valid range, block everything */
                                r = seccomp_rule_add_exact(
                                                seccomp,
                                                SCMP_ACT_ERRNO(EAFNOSUPPORT),
                                                SCMP_SYS(socket),
                                                0);
                                if (r < 0) {
                                        log_debug_errno(r, "Failed to add socket() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                                        continue;
                                }

                        } else {

                                /* Block everything below the first entry */
                                r = seccomp_rule_add_exact(
                                                seccomp,
                                                SCMP_ACT_ERRNO(EAFNOSUPPORT),
                                                SCMP_SYS(socket),
                                                1,
                                                SCMP_A0(SCMP_CMP_LT, first));
                                if (r < 0) {
                                        log_debug_errno(r, "Failed to add socket() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                                        continue;
                                }

                                /* Block everything above the last entry */
                                r = seccomp_rule_add_exact(
                                                seccomp,
                                                SCMP_ACT_ERRNO(EAFNOSUPPORT),
                                                SCMP_SYS(socket),
                                                1,
                                                SCMP_A0(SCMP_CMP_GT, last));
                                if (r < 0) {
                                        log_debug_errno(r, "Failed to add socket() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                                        continue;
                                }

                                /* Block everything between the first and last entry */
                                for (af = 1; af < af_max(); af++) {

                                        if (set_contains(address_families, INT_TO_PTR(af)))
                                                continue;

                                        r = seccomp_rule_add_exact(
                                                        seccomp,
                                                        SCMP_ACT_ERRNO(EAFNOSUPPORT),
                                                        SCMP_SYS(socket),
                                                        1,
                                                        SCMP_A0(SCMP_CMP_EQ, af));
                                        if (r < 0)
                                                break;
                                }
                                if (r < 0) {
                                        log_debug_errno(r, "Failed to add socket() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                                        continue;
                                }
                        }

                } else {
                        void *af;

                        /* If this is a blacklist, then generate one rule for
                         * each address family that are then combined in OR
                         * checks. */

                        SET_FOREACH(af, address_families, i) {

                                r = seccomp_rule_add_exact(
                                                seccomp,
                                                SCMP_ACT_ERRNO(EAFNOSUPPORT),
                                                SCMP_SYS(socket),
                                                1,
                                                SCMP_A0(SCMP_CMP_EQ, PTR_TO_INT(af)));
                                if (r < 0)
                                        break;
                        }
                        if (r < 0) {
                                log_debug_errno(r, "Failed to add socket() rule for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
                                continue;
                        }
                }

                r = seccomp_load(seccomp);
                if (IN_SET(r, -EPERM, -EACCES))
                        return r;
                if (r < 0)
                        log_debug_errno(r, "Failed to install socket family rules for architecture %s, skipping: %m", seccomp_arch_to_string(arch));
        }

        return 0;
}