string_replace(PyStringObject *self, PyObject *args)
{
    Py_ssize_t count = -1;
    PyObject *from, *to;
    const char *from_s, *to_s;
    Py_ssize_t from_len, to_len;

    if (!PyArg_ParseTuple(args, "OO|n:replace", &from, &to, &count))
        return NULL;

    if (PyString_Check(from)) {
        from_s = PyString_AS_STRING(from);
        from_len = PyString_GET_SIZE(from);
    }
#ifdef Py_USING_UNICODE
    if (PyUnicode_Check(from))
        return PyUnicode_Replace((PyObject *)self,
                                 from, to, count);
#endif
    else if (PyObject_AsCharBuffer(from, &from_s, &from_len))
        return NULL;

    if (PyString_Check(to)) {
        to_s = PyString_AS_STRING(to);
        to_len = PyString_GET_SIZE(to);
    }
#ifdef Py_USING_UNICODE
    else if (PyUnicode_Check(to))
        return PyUnicode_Replace((PyObject *)self,
                                 from, to, count);
#endif
    else if (PyObject_AsCharBuffer(to, &to_s, &to_len))
        return NULL;

    return (PyObject *)replace((PyStringObject *) self,
                               from_s, from_len,
                               to_s, to_len, count);
}