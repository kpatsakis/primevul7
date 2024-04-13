string_item(PyStringObject *a, register Py_ssize_t i)
{
    char pchar;
    PyObject *v;
    if (i < 0 || i >= Py_SIZE(a)) {
        PyErr_SetString(PyExc_IndexError, "string index out of range");
        return NULL;
    }
    pchar = a->ob_sval[i];
    v = (PyObject *)characters[pchar & UCHAR_MAX];
    if (v == NULL)
        v = PyString_FromStringAndSize(&pchar, 1);
    else {
#ifdef COUNT_ALLOCS
        one_strings++;
#endif
        Py_INCREF(v);
    }
    return v;
}