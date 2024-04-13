static bool is_seccomp_filter_available(void) {
        return prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, NULL, 0, 0) < 0 &&
                errno == EFAULT;
}