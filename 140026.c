bool is_seccomp_available(void) {
        static int cached_enabled = -1;

        if (cached_enabled < 0)
                cached_enabled =
                        is_basic_seccomp_available() &&
                        is_seccomp_filter_available();

        return cached_enabled;
}