string_getnewargs(PyStringObject *v)
{
    return Py_BuildValue("(s#)", v->ob_sval, Py_SIZE(v));
}