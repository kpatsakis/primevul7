warn_invalid_escape_sequence(struct compiling *c, const node *n,
                             unsigned char first_invalid_escape_char)
{
    PyObject *msg = PyUnicode_FromFormat("invalid escape sequence \\%c",
                                         first_invalid_escape_char);
    if (msg == NULL) {
        return -1;
    }
    if (PyErr_WarnExplicitObject(PyExc_DeprecationWarning, msg,
                                   c->c_filename, LINENO(n),
                                   NULL, NULL) < 0)
    {
        if (PyErr_ExceptionMatches(PyExc_DeprecationWarning)) {
            const char *s;

            /* Replace the DeprecationWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();

            s = PyUnicode_AsUTF8(msg);
            if (s != NULL) {
                ast_error(c, n, s);
            }
        }
        Py_DECREF(msg);
        return -1;
    }
    Py_DECREF(msg);
    return 0;
}