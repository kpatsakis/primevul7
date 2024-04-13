_string_tailmatch(PyStringObject *self, PyObject *substr, Py_ssize_t start,
                  Py_ssize_t end, int direction)
{
    Py_ssize_t len = PyString_GET_SIZE(self);
    Py_ssize_t slen;
    const char* sub;
    const char* str;

    if (PyString_Check(substr)) {
        sub = PyString_AS_STRING(substr);
        slen = PyString_GET_SIZE(substr);
    }
#ifdef Py_USING_UNICODE
    else if (PyUnicode_Check(substr))
        return PyUnicode_Tailmatch((PyObject *)self,
                                   substr, start, end, direction);
#endif
    else if (PyObject_AsCharBuffer(substr, &sub, &slen))
        return -1;
    str = PyString_AS_STRING(self);

    ADJUST_INDICES(start, end, len);

    if (direction < 0) {
        /* startswith */
        if (start+slen > len)
            return 0;
    } else {
        /* endswith */
        if (end-start < slen || start > len)
            return 0;

        if (end-slen > start)
            start = end - slen;
    }
    if (end-start >= slen)
        return ! memcmp(str+start, sub, slen);
    return 0;
}