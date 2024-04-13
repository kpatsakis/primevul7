PyString_Concat(register PyObject **pv, register PyObject *w)
{
    register PyObject *v;
    if (*pv == NULL)
        return;
    if (w == NULL || !PyString_Check(*pv)) {
        Py_CLEAR(*pv);
        return;
    }
    v = string_concat((PyStringObject *) *pv, w);
    Py_SETREF(*pv, v);
}