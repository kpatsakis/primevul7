do_xstrip(PyStringObject *self, int striptype, PyObject *sepobj)
{
    char *s = PyString_AS_STRING(self);
    Py_ssize_t len = PyString_GET_SIZE(self);
    char *sep = PyString_AS_STRING(sepobj);
    Py_ssize_t seplen = PyString_GET_SIZE(sepobj);
    Py_ssize_t i, j;

    i = 0;
    if (striptype != RIGHTSTRIP) {
        while (i < len && memchr(sep, Py_CHARMASK(s[i]), seplen)) {
            i++;
        }
    }

    j = len;
    if (striptype != LEFTSTRIP) {
        do {
            j--;
        } while (j >= i && memchr(sep, Py_CHARMASK(s[j]), seplen));
        j++;
    }

    if (i == 0 && j == len && PyString_CheckExact(self)) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    else
        return PyString_FromStringAndSize(s+i, j-i);
}