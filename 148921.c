decode_bytes_with_escapes(struct compiling *c, const node *n, const char *s,
                          size_t len)
{
    const char *first_invalid_escape;
    PyObject *result = _PyBytes_DecodeEscape(s, len, NULL, 0, NULL,
                                             &first_invalid_escape);
    if (result == NULL)
        return NULL;

    if (first_invalid_escape != NULL) {
        if (warn_invalid_escape_sequence(c, n, *first_invalid_escape) < 0) {
            Py_DECREF(result);
            return NULL;
        }
    }
    return result;
}