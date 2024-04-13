int seccomp_restrict_archs(Set *archs) {
        _cleanup_(seccomp_releasep) scmp_filter_ctx seccomp = NULL;
        Iterator i;
        void *id;
        int r;

        /* This installs a filter with no rules, but that restricts the system call architectures to the specified
         * list. */

        seccomp = seccomp_init(SCMP_ACT_ALLOW);
        if (!seccomp)
                return -ENOMEM;

        SET_FOREACH(id, archs, i) {
                r = seccomp_arch_add(seccomp, PTR_TO_UINT32(id) - 1);
                if (r == -EEXIST)
                        continue;
                if (r < 0)
                        return r;
        }

        r = seccomp_attr_set(seccomp, SCMP_FLTATR_CTL_NNP, 0);
        if (r < 0)
                return r;

        r = seccomp_load(seccomp);
        if (IN_SET(r, -EPERM, -EACCES))
                return r;
        if (r < 0)
                log_debug_errno(r, "Failed to restrict system call architectures, skipping: %m");

        return 0;
}