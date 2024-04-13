fstring_find_literal_and_expr(const char **str, const char *end, int raw,
                              int recurse_lvl, PyObject **literal,
                              expr_ty *expression,
                              struct compiling *c, const node *n)
{
    int result;

    assert(*literal == NULL && *expression == NULL);

    /* Get any literal string. */
    result = fstring_find_literal(str, end, raw, literal, recurse_lvl, c, n);
    if (result < 0)
        goto error;

    assert(result == 0 || result == 1);

    if (result == 1)
        /* We have a literal, but don't look at the expression. */
        return 1;

    if (*str >= end || **str == '}')
        /* We're at the end of the string or the end of a nested
           f-string: no expression. The top-level error case where we
           expect to be at the end of the string but we're at a '}' is
           handled later. */
        return 0;

    /* We must now be the start of an expression, on a '{'. */
    assert(**str == '{');

    if (fstring_find_expr(str, end, raw, recurse_lvl, expression, c, n) < 0)
        goto error;

    return 0;

error:
    Py_CLEAR(*literal);
    return -1;
}