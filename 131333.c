string_getsize(register PyObject *op)
{
    char *s;
    Py_ssize_t len;
    if (PyString_AsStringAndSize(op, &s, &len))
        return -1;
    return len;
}