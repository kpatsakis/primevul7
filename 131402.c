_PyString_FormatLong(PyObject *val, int flags, int prec, int type,
                     char **pbuf, int *plen)
{
    PyObject *result = NULL, *r1;
    const char *s;
    char *buf;
    Py_ssize_t i;
    int sign;           /* 1 if '-', else 0 */
    int len;            /* number of characters */
    Py_ssize_t llen;
    int numdigits;      /* len == numnondigits + skipped + numdigits */
    int numnondigits, skipped, filled;
    const char *method;

    switch (type) {
    case 'd':
    case 'u':
        method = "str";
        result = Py_TYPE(val)->tp_str(val);
        break;
    case 'o':
        method = "oct";
        result = Py_TYPE(val)->tp_as_number->nb_oct(val);
        break;
    case 'x':
    case 'X':
        method = "hex";
        result = Py_TYPE(val)->tp_as_number->nb_hex(val);
        break;
    default:
        assert(!"'type' not in [duoxX]");
    }
    if (!result)
        return NULL;

    if (PyString_AsStringAndSize(result, (char **)&s, &llen) < 0) {
        Py_DECREF(result);
        return NULL;
    }
    if (llen > INT_MAX) {
        PyErr_SetString(PyExc_ValueError, "string too large in _PyString_FormatLong");
        Py_DECREF(result);
        return NULL;
    }
    len = (int)llen;
    if (len > 0 && s[len-1] == 'L') {
        --len;
        if (len == 0)
            goto error;
    }
    sign = s[0] == '-';
    numnondigits = sign;

    /* Need to skip 0x, 0X or 0. */
    skipped = 0;
    switch (type) {
    case 'o':
        if (s[sign] != '0')
            goto error;
        /* If 0 is only digit, leave it alone. */
        if ((flags & F_ALT) == 0 && len - sign > 1)
            skipped = 1;
        break;
    case 'x':
    case 'X':
        if (s[sign] != '0' || (s[sign + 1] != 'x' && s[sign + 1] != 'X'))
            goto error;
        if ((flags & F_ALT) == 0)
            skipped = 2;
        else
            numnondigits += 2;
        break;
    }
    numdigits = len - numnondigits - skipped;
    if (numdigits <= 0)
        goto error;

    filled = prec - numdigits;
    if (filled < 0)
        filled = 0;
    len = numnondigits + filled + numdigits;

    /* To modify the string in-place, there can only be one reference. */
    if (skipped >= filled &&
        PyString_CheckExact(result) &&
        Py_REFCNT(result) == 1 &&
        !PyString_CHECK_INTERNED(result))
    {
        r1 = NULL;
        buf = (char *)s + skipped - filled;
    }
    else {
        r1 = result;
        result = PyString_FromStringAndSize(NULL, len);
        if (!result) {
            Py_DECREF(r1);
            return NULL;
        }
        buf = PyString_AS_STRING(result);
    }

    for (i = numnondigits; --i >= 0;)
        buf[i] = s[i];
    buf += numnondigits;
    s += numnondigits + skipped;
    for (i = 0; i < filled; i++)
        *buf++ = '0';
    if (r1 == NULL) {
        assert(buf == s);
        buf += numdigits;
    }
    else {
        for (i = 0; i < numdigits; i++)
            *buf++ = *s++;
    }
    *buf = '\0';
    buf -= len;
    Py_XDECREF(r1);

    /* Fix up case for hex conversions. */
    if (type == 'X') {
        /* Need to convert all lower case letters to upper case.
           and need to convert 0x to 0X (and -0x to -0X). */
        for (i = 0; i < len; i++) {
            if (buf[i] >= 'a' && buf[i] <= 'z')
                buf[i] -= 'a'-'A';
        }
    }
    *pbuf = buf;
    *plen = len;
    return result;

error:
    PyErr_Format(PyExc_ValueError,
                 "%%%c format: invalid result of __%s__ (type=%.200s)",
                 type, method, Py_TYPE(val)->tp_name);
    Py_DECREF(result);
    return NULL;
}