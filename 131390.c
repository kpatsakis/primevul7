str_subtype_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyObject *tmp, *pnew;
    Py_ssize_t n;

    assert(PyType_IsSubtype(type, &PyString_Type));
    tmp = string_new(&PyString_Type, args, kwds);
    if (tmp == NULL)
        return NULL;
    assert(PyString_Check(tmp));
    n = PyString_GET_SIZE(tmp);
    pnew = type->tp_alloc(type, n);
    if (pnew != NULL) {
        Py_MEMCPY(PyString_AS_STRING(pnew), PyString_AS_STRING(tmp), n+1);
        ((PyStringObject *)pnew)->ob_shash =
            ((PyStringObject *)tmp)->ob_shash;
        ((PyStringObject *)pnew)->ob_sstate = SSTATE_NOT_INTERNED;
    }
    Py_DECREF(tmp);
    return pnew;
}