_PyUnicode_DecodeUnicodeEscape(const char *s,
                               Py_ssize_t size,
                               const char *errors,
                               const char **first_invalid_escape)
{
    *first_invalid_escape = NULL;
    return PyUnicode_DecodeUnicodeEscape(s, size, errors);
}