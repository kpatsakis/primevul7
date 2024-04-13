static int add_seccomp_syscall_filter(scmp_filter_ctx seccomp,
                                      uint32_t arch,
                                      int nr,
                                      unsigned int arg_cnt,
                                      const struct scmp_arg_cmp arg) {
        int r;

        r = seccomp_rule_add_exact(seccomp, SCMP_ACT_ERRNO(EPERM), nr, arg_cnt, arg);
        if (r < 0) {
                _cleanup_free_ char *n = NULL;

                n = seccomp_syscall_resolve_num_arch(arch, nr);
                log_debug_errno(r, "Failed to add %s() rule for architecture %s, skipping: %m",
                                strna(n),
                                seccomp_arch_to_string(arch));
        }

        return r;
}