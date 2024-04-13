_PyBytes_DecodeEscape(const char *s,
                      Py_ssize_t len,
                      const char *errors,
                      Py_ssize_t unicode,
                      const char *recode_encoding,
                      const char **first_invalid_escape)
{
    *first_invalid_escape = NULL;
    return PyBytes_DecodeEscape(s, len, errors, unicode, recode_encoding);
}