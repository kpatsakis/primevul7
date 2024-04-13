string_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyObject *x = NULL;
    static char *kwlist[] = {"object", 0};

    if (type != &PyString_Type)
        return str_subtype_new(type, args, kwds);
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O:str", kwlist, &x))
        return NULL;
    if (x == NULL)
        return PyString_FromString("");
    return PyObject_Str(x);
}