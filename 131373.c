string_upper(PyStringObject *self)
{
    char *s;
    Py_ssize_t i, n = PyString_GET_SIZE(self);
    PyObject *newobj;

    newobj = PyString_FromStringAndSize(NULL, n);
    if (!newobj)
        return NULL;

    s = PyString_AS_STRING(newobj);

    Py_MEMCPY(s, PyString_AS_STRING(self), n);

    for (i = 0; i < n; i++) {
        int c = Py_CHARMASK(s[i]);
        if (islower(c))
            s[i] = _toupper(c);
    }

    return newobj;
}