string_getbuffer(register PyObject *op)
{
    char *s;
    Py_ssize_t len;
    if (PyString_AsStringAndSize(op, &s, &len))
        return NULL;
    return s;
}