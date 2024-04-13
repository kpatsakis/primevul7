ast_error(struct compiling *c, const node *n, const char *errmsg)
{
    PyObject *value, *errstr, *loc, *tmp;

    loc = PyErr_ProgramTextObject(c->c_filename, LINENO(n));
    if (!loc) {
        Py_INCREF(Py_None);
        loc = Py_None;
    }
    tmp = Py_BuildValue("(OiiN)", c->c_filename, LINENO(n), n->n_col_offset, loc);
    if (!tmp)
        return 0;
    errstr = PyUnicode_FromString(errmsg);
    if (!errstr) {
        Py_DECREF(tmp);
        return 0;
    }
    value = PyTuple_Pack(2, errstr, tmp);
    Py_DECREF(errstr);
    Py_DECREF(tmp);
    if (value) {
        PyErr_SetObject(PyExc_SyntaxError, value);
        Py_DECREF(value);
    }
    return 0;
}