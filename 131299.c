string_capitalize(PyStringObject *self)
{
    char *s = PyString_AS_STRING(self), *s_new;
    Py_ssize_t i, n = PyString_GET_SIZE(self);
    PyObject *newobj;

    newobj = PyString_FromStringAndSize(NULL, n);
    if (newobj == NULL)
        return NULL;
    s_new = PyString_AsString(newobj);
    if (0 < n) {
        int c = Py_CHARMASK(*s++);
        if (islower(c))
            *s_new = toupper(c);
        else
            *s_new = c;
        s_new++;
    }
    for (i = 1; i < n; i++) {
        int c = Py_CHARMASK(*s++);
        if (isupper(c))
            *s_new = tolower(c);
        else
            *s_new = c;
        s_new++;
    }
    return newobj;
}