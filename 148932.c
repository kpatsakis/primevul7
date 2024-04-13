ast_for_call(struct compiling *c, const node *n, expr_ty func, bool allowgen)
{
    /*
      arglist: argument (',' argument)*  [',']
      argument: ( test [comp_for] | '*' test | test '=' test | '**' test )
    */

    int i, nargs, nkeywords;
    int ndoublestars;
    asdl_seq *args;
    asdl_seq *keywords;

    REQ(n, arglist);

    nargs = 0;
    nkeywords = 0;
    for (i = 0; i < NCH(n); i++) {
        node *ch = CHILD(n, i);
        if (TYPE(ch) == argument) {
            if (NCH(ch) == 1)
                nargs++;
            else if (TYPE(CHILD(ch, 1)) == comp_for) {
                nargs++;
                if (!allowgen) {
                    ast_error(c, ch, "invalid syntax");
                    return NULL;
                }
                if (NCH(n) > 1) {
                    ast_error(c, ch, "Generator expression must be parenthesized");
                    return NULL;
                }
            }
            else if (TYPE(CHILD(ch, 0)) == STAR)
                nargs++;
            else
                /* TYPE(CHILD(ch, 0)) == DOUBLESTAR or keyword argument */
                nkeywords++;
        }
    }

    args = _Ta3_asdl_seq_new(nargs, c->c_arena);
    if (!args)
        return NULL;
    keywords = _Ta3_asdl_seq_new(nkeywords, c->c_arena);
    if (!keywords)
        return NULL;

    nargs = 0;  /* positional arguments + iterable argument unpackings */
    nkeywords = 0;  /* keyword arguments + keyword argument unpackings */
    ndoublestars = 0;  /* just keyword argument unpackings */
    for (i = 0; i < NCH(n); i++) {
        node *ch = CHILD(n, i);
        if (TYPE(ch) == argument) {
            expr_ty e;
            node *chch = CHILD(ch, 0);
            if (NCH(ch) == 1) {
                /* a positional argument */
                if (nkeywords) {
                    if (ndoublestars) {
                        ast_error(c, chch,
                                "positional argument follows "
                                "keyword argument unpacking");
                    }
                    else {
                        ast_error(c, chch,
                                "positional argument follows "
                                "keyword argument");
                    }
                    return NULL;
                }
                e = ast_for_expr(c, chch);
                if (!e)
                    return NULL;
                asdl_seq_SET(args, nargs++, e);
            }
            else if (TYPE(chch) == STAR) {
                /* an iterable argument unpacking */
                expr_ty starred;
                if (ndoublestars) {
                    ast_error(c, chch,
                            "iterable argument unpacking follows "
                            "keyword argument unpacking");
                    return NULL;
                }
                e = ast_for_expr(c, CHILD(ch, 1));
                if (!e)
                    return NULL;
                starred = Starred(e, Load, LINENO(chch),
                        chch->n_col_offset,
                        c->c_arena);
                if (!starred)
                    return NULL;
                asdl_seq_SET(args, nargs++, starred);

            }
            else if (TYPE(chch) == DOUBLESTAR) {
                /* a keyword argument unpacking */
                keyword_ty kw;
                i++;
                e = ast_for_expr(c, CHILD(ch, 1));
                if (!e)
                    return NULL;
                kw = keyword(NULL, e, c->c_arena);
                asdl_seq_SET(keywords, nkeywords++, kw);
                ndoublestars++;
            }
            else if (TYPE(CHILD(ch, 1)) == comp_for) {
                /* the lone generator expression */
                e = ast_for_genexp(c, ch);
                if (!e)
                    return NULL;
                asdl_seq_SET(args, nargs++, e);
            }
            else {
                /* a keyword argument */
                keyword_ty kw;
                identifier key, tmp;
                int k;

                /* chch is test, but must be an identifier? */
                e = ast_for_expr(c, chch);
                if (!e)
                    return NULL;
                /* f(lambda x: x[0] = 3) ends up getting parsed with
                 * LHS test = lambda x: x[0], and RHS test = 3.
                 * SF bug 132313 points out that complaining about a keyword
                 * then is very confusing.
                 */
                if (e->kind == Lambda_kind) {
                    ast_error(c, chch,
                            "lambda cannot contain assignment");
                    return NULL;
                }
                else if (e->kind != Name_kind) {
                    ast_error(c, chch,
                            "keyword can't be an expression");
                    return NULL;
                }
                else if (forbidden_name(c, e->v.Name.id, ch, 1)) {
                    return NULL;
                }
                key = e->v.Name.id;
                for (k = 0; k < nkeywords; k++) {
                    tmp = ((keyword_ty)asdl_seq_GET(keywords, k))->arg;
                    if (tmp && !PyUnicode_Compare(tmp, key)) {
                        ast_error(c, chch,
                                "keyword argument repeated");
                        return NULL;
                    }
                }
                e = ast_for_expr(c, CHILD(ch, 2));
                if (!e)
                    return NULL;
                kw = keyword(key, e, c->c_arena);
                if (!kw)
                    return NULL;
                asdl_seq_SET(keywords, nkeywords++, kw);
            }
        }
    }

    return Call(func, args, keywords, func->lineno, func->col_offset, c->c_arena);
}