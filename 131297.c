string_title(PyStringObject *self)
{
    char *s = PyString_AS_STRING(self), *s_new;
    Py_ssize_t i, n = PyString_GET_SIZE(self);
    int previous_is_cased = 0;
    PyObject *newobj;

    newobj = PyString_FromStringAndSize(NULL, n);
    if (newobj == NULL)
        return NULL;
    s_new = PyString_AsString(newobj);
    for (i = 0; i < n; i++) {
        int c = Py_CHARMASK(*s++);
        if (islower(c)) {
            if (!previous_is_cased)
                c = toupper(c);
            previous_is_cased = 1;
        } else if (isupper(c)) {
            if (previous_is_cased)
                c = tolower(c);
            previous_is_cased = 1;
        } else
            previous_is_cased = 0;
        *s_new++ = c;
    }
    return newobj;
}