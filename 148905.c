_PyObject_FastCall(PyObject *func, PyObject *const *args, int nargs)
{
    PyObject *t, *res;
    int i;

    t = PyTuple_New(nargs);
    if (t == NULL) {
        return NULL;
    }
    for (i = 0; i < nargs; i++) {
        if (PyTuple_SetItem(t, i, args[i]) < 0) {
            Py_DECREF(t);
            return NULL;
        }
    }
    res = PyObject_CallObject(func, t);
    Py_DECREF(t);
    return res;
}