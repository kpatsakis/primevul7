static int parse_smacro_template(Token ***tpp, SMacro *tmpl)
{
    int nparam = 0;
    enum sparmflags flags;
    struct smac_param *params = NULL;
    bool err, done;
    bool greedy = false;
    Token **tn = *tpp;
    Token *t = *tn;
    Token *name;

    /*
     * DO NOT skip whitespace here, or we won't be able to distinguish:
     *
     * %define foo (a,b)		; no arguments, (a,b) is the expansion
     * %define bar(a,b)			; two arguments, empty expansion
     *
     * This ambiguity was inherited from C.
     */

    if (!tok_is(t, '('))
        goto finish;

    if (tmpl) {
        Token *tx = t;
        Token **txpp = &tx;
        int sparam;

        /* Count parameters first */
        sparam = parse_smacro_template(&txpp, NULL);
        if (!sparam)
            goto finish;        /* No parameters, we're done */
        nasm_newn(params, sparam);
    }

    /* Skip leading paren */
    tn = &t->next;
    t = *tn;

    name = NULL;
    flags = 0;
    err = done = false;

    while (!done) {
        if (!t || !t->type) {
            if (name || flags)
                nasm_nonfatal("`)' expected to terminate macro template");
            else
                nasm_nonfatal("parameter identifier expected");
            break;
        }

        switch (t->type) {
        case TOK_ID:
            if (name)
                goto bad;
            name = t;
            break;

        case TOK_OTHER:
            if (t->len != 1)
                goto bad;
            switch (t->text.a[0]) {
            case '=':
                flags |= SPARM_EVAL;
                break;
            case '&':
                flags |= SPARM_STR;
                break;
            case '!':
                flags |= SPARM_NOSTRIP;
                break;
            case '+':
                flags |= SPARM_GREEDY;
                greedy = true;
                break;
            case ',':
                if (greedy)
                    nasm_nonfatal("greedy parameter must be last");
                /* fall through */
            case ')':
                if (params) {
                    if (name)
			steal_Token(&params[nparam].name, name);
                    params[nparam].flags = flags;
                }
                nparam++;
                name = NULL;
                flags = 0;
                done = t->text.a[0] == ')';
                break;
            default:
                goto bad;
            }
            break;

        case TOK_WHITESPACE:
            break;

        default:
        bad:
            if (!err) {
                nasm_nonfatal("garbage `%s' in macro parameter list", tok_text(t));
                err = true;
            }
            break;
        }

        tn = &t->next;
        t = *tn;
    }

finish:
    while (t && t->type == TOK_WHITESPACE) {
        tn = &t->next;
        t = t->next;
    }
    *tpp = tn;
    if (tmpl) {
        tmpl->nparam = nparam;
        tmpl->greedy = greedy;
        tmpl->params = params;
    }
    return nparam;
}