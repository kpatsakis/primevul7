string_richcompare(PyStringObject *a, PyStringObject *b, int op)
{
    int c;
    Py_ssize_t len_a, len_b;
    Py_ssize_t min_len;
    PyObject *result;

    /* Make sure both arguments are strings. */
    if (!(PyString_Check(a) && PyString_Check(b))) {
        result = Py_NotImplemented;
        goto out;
    }
    if (a == b) {
        switch (op) {
        case Py_EQ:case Py_LE:case Py_GE:
            result = Py_True;
            goto out;
        case Py_NE:case Py_LT:case Py_GT:
            result = Py_False;
            goto out;
        }
    }
    if (op == Py_EQ) {
        /* Supporting Py_NE here as well does not save
           much time, since Py_NE is rarely used.  */
        if (Py_SIZE(a) == Py_SIZE(b)
            && (a->ob_sval[0] == b->ob_sval[0]
            && memcmp(a->ob_sval, b->ob_sval, Py_SIZE(a)) == 0)) {
            result = Py_True;
        } else {
            result = Py_False;
        }
        goto out;
    }
    len_a = Py_SIZE(a); len_b = Py_SIZE(b);
    min_len = (len_a < len_b) ? len_a : len_b;
    if (min_len > 0) {
        c = Py_CHARMASK(*a->ob_sval) - Py_CHARMASK(*b->ob_sval);
        if (c==0)
            c = memcmp(a->ob_sval, b->ob_sval, min_len);
    } else
        c = 0;
    if (c == 0)
        c = (len_a < len_b) ? -1 : (len_a > len_b) ? 1 : 0;
    switch (op) {
    case Py_LT: c = c <  0; break;
    case Py_LE: c = c <= 0; break;
    case Py_EQ: assert(0);  break; /* unreachable */
    case Py_NE: c = c != 0; break;
    case Py_GT: c = c >  0; break;
    case Py_GE: c = c >= 0; break;
    default:
        result = Py_NotImplemented;
        goto out;
    }
    result = c ? Py_True : Py_False;
  out:
    Py_INCREF(result);
    return result;
}