static bool is_basic_seccomp_available(void) {
        return prctl(PR_GET_SECCOMP, 0, 0, 0, 0) >= 0;
}