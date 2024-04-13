string_slice(register PyStringObject *a, register Py_ssize_t i,
             register Py_ssize_t j)
     /* j -- may be negative! */
{
    if (i < 0)
        i = 0;
    if (j < 0)
        j = 0; /* Avoid signed/unsigned bug in next line */
    if (j > Py_SIZE(a))
        j = Py_SIZE(a);
    if (i == 0 && j == Py_SIZE(a) && PyString_CheckExact(a)) {
        /* It's the same as a */
        Py_INCREF(a);
        return (PyObject *)a;
    }
    if (j < i)
        j = i;
    return PyString_FromStringAndSize(a->ob_sval + i, j-i);
}