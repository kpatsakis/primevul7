psutil_posix_getpriority(PyObject *self, PyObject *args) {
    long pid;
    int priority;
    errno = 0;

    if (! PyArg_ParseTuple(args, "l", &pid))
        return NULL;

#ifdef PSUTIL_OSX
    priority = getpriority(PRIO_PROCESS, (id_t)pid);
#else
    priority = getpriority(PRIO_PROCESS, pid);
#endif
    if (errno != 0)
        return PyErr_SetFromErrno(PyExc_OSError);
    return Py_BuildValue("i", priority);
}