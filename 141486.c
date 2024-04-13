psutil_pid_exists(long pid) {
    int ret;

    // No negative PID exists, plus -1 is an alias for sending signal
    // too all processes except system ones. Not what we want.
    if (pid < 0)
        return 0;

    // As per "man 2 kill" PID 0 is an alias for sending the signal to
    // every process in the process group of the calling process.
    // Not what we want. Some platforms have PID 0, some do not.
    // We decide that at runtime.
    if (pid == 0) {
#if defined(PSUTIL_LINUX) || defined(PSUTIL_FREEBSD)
        return 0;
#else
        return 1;
#endif
    }

#if defined(PSUTIL_OSX)
    ret = kill((pid_t)pid , 0);
#else
    ret = kill(pid , 0);
#endif

    if (ret == 0)
        return 1;
    else {
        if (errno == ESRCH) {
            // ESRCH == No such process
            return 0;
        }
        else if (errno == EPERM) {
            // EPERM clearly indicates there's a process to deny
            // access to.
            return 1;
        }
        else {
            // According to "man 2 kill" possible error values are
            // (EINVAL, EPERM, ESRCH) therefore we should never get
            // here. If we do let's be explicit in considering this
            // an error.
            PyErr_SetFromErrno(PyExc_OSError);
            return -1;
        }
    }
}