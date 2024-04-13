string_swapcase(PyStringObject *self)
{
    char *s = PyString_AS_STRING(self), *s_new;
    Py_ssize_t i, n = PyString_GET_SIZE(self);
    PyObject *newobj;

    newobj = PyString_FromStringAndSize(NULL, n);
    if (newobj == NULL)
        return NULL;
    s_new = PyString_AsString(newobj);
    for (i = 0; i < n; i++) {
        int c = Py_CHARMASK(*s++);
        if (islower(c)) {
            *s_new = toupper(c);
        }
        else if (isupper(c)) {
            *s_new = tolower(c);
        }
        else
            *s_new = c;
        s_new++;
    }
    return newobj;
}