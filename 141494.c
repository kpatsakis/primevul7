psutil_posix_setpriority(PyObject *self, PyObject *args) {
    long pid;
    int priority;
    int retval;

    if (! PyArg_ParseTuple(args, "li", &pid, &priority))
        return NULL;

#ifdef PSUTIL_OSX
    retval = setpriority(PRIO_PROCESS, (id_t)pid, priority);
#else
    retval = setpriority(PRIO_PROCESS, pid, priority);
#endif
    if (retval == -1)
        return PyErr_SetFromErrno(PyExc_OSError);
    Py_RETURN_NONE;
}