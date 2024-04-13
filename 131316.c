PyString_Repr(PyObject *obj, int smartquotes)
{
    register PyStringObject* op = (PyStringObject*) obj;
    size_t newsize;
    PyObject *v;
    if (Py_SIZE(op) > (PY_SSIZE_T_MAX - 2)/4) {
        PyErr_SetString(PyExc_OverflowError,
            "string is too large to make repr");
        return NULL;
    }
    newsize = 2 + 4*Py_SIZE(op);
    v = PyString_FromStringAndSize((char *)NULL, newsize);
    if (v == NULL) {
        return NULL;
    }
    else {
        register Py_ssize_t i;
        register char c;
        register char *p;
        int quote;

        /* figure out which quote to use; single is preferred */
        quote = '\'';
        if (smartquotes &&
            memchr(op->ob_sval, '\'', Py_SIZE(op)) &&
            !memchr(op->ob_sval, '"', Py_SIZE(op)))
            quote = '"';

        p = PyString_AS_STRING(v);
        *p++ = quote;
        for (i = 0; i < Py_SIZE(op); i++) {
            /* There's at least enough room for a hex escape
               and a closing quote. */
            assert(newsize - (p - PyString_AS_STRING(v)) >= 5);
            c = op->ob_sval[i];
            if (c == quote || c == '\\')
                *p++ = '\\', *p++ = c;
            else if (c == '\t')
                *p++ = '\\', *p++ = 't';
            else if (c == '\n')
                *p++ = '\\', *p++ = 'n';
            else if (c == '\r')
                *p++ = '\\', *p++ = 'r';
            else if (c < ' ' || c >= 0x7f) {
                /* For performance, we don't want to call
                   PyOS_snprintf here (extra layers of
                   function call). */
                sprintf(p, "\\x%02x", c & 0xff);
                p += 4;
            }
            else
                *p++ = c;
        }
        assert(newsize - (p - PyString_AS_STRING(v)) >= 1);
        *p++ = quote;
        *p = '\0';
        if (_PyString_Resize(&v, (p - PyString_AS_STRING(v))))
            return NULL;
        return v;
    }
}