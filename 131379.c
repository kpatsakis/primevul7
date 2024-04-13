string_mod(PyObject *v, PyObject *w)
{
    if (!PyString_Check(v)) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }
    return PyString_Format(v, w);
}