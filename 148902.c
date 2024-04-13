ast_for_dictelement(struct compiling *c, const node *n, int *i,
                    expr_ty *key, expr_ty *value)
{
    expr_ty expression;
    if (TYPE(CHILD(n, *i)) == DOUBLESTAR) {
        assert(NCH(n) - *i >= 2);

        expression = ast_for_expr(c, CHILD(n, *i + 1));
        if (!expression)
            return 0;
        *key = NULL;
        *value = expression;

        *i += 2;
    }
    else {
        assert(NCH(n) - *i >= 3);

        expression = ast_for_expr(c, CHILD(n, *i));
        if (!expression)
            return 0;
        *key = expression;

        REQ(CHILD(n, *i + 1), COLON);

        expression = ast_for_expr(c, CHILD(n, *i + 2));
        if (!expression)
            return 0;
        *value = expression;

        *i += 3;
    }
    return 1;
}