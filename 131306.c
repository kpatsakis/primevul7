PyString_AsString(register PyObject *op)
{
    if (!PyString_Check(op))
        return string_getbuffer(op);
    return ((PyStringObject *)op) -> ob_sval;
}