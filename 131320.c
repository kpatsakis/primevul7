do_strip(PyStringObject *self, int striptype)
{
    char *s = PyString_AS_STRING(self);
    Py_ssize_t len = PyString_GET_SIZE(self), i, j;

    i = 0;
    if (striptype != RIGHTSTRIP) {
        while (i < len && isspace(Py_CHARMASK(s[i]))) {
            i++;
        }
    }

    j = len;
    if (striptype != LEFTSTRIP) {
        do {
            j--;
        } while (j >= i && isspace(Py_CHARMASK(s[j])));
        j++;
    }

    if (i == 0 && j == len && PyString_CheckExact(self)) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    else
        return PyString_FromStringAndSize(s+i, j-i);
}