psutil_raise_for_pid(long pid, char *syscall_name) {
    // Set exception to AccessDenied if pid exists else NoSuchProcess.
    if (errno != 0) {
        // Unlikely we get here.
        PyErr_SetFromErrno(PyExc_OSError);
        return 0;
    }
    else if (psutil_pid_exists(pid) == 0) {
        psutil_debug("%s syscall failed and PID %i no longer exists; "
                     "assume NoSuchProcess", syscall_name, pid);
        NoSuchProcess("");
    }
    else {
        PyErr_Format(PyExc_RuntimeError, "%s syscall failed", syscall_name);
    }
    return 0;
}