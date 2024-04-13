_PyString_Eq(PyObject *o1, PyObject *o2)
{
    PyStringObject *a = (PyStringObject*) o1;
    PyStringObject *b = (PyStringObject*) o2;
    return Py_SIZE(a) == Py_SIZE(b)
      && memcmp(a->ob_sval, b->ob_sval, Py_SIZE(a)) == 0;
}