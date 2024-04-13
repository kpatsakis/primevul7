PyString_ConcatAndDel(register PyObject **pv, register PyObject *w)
{
    PyString_Concat(pv, w);
    Py_XDECREF(w);
}