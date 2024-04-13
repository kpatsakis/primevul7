static enum cond_state if_condition(Token * tline, enum preproc_token ct)
{
    bool j;
    Token *t, *tt, *origline;
    struct ppscan pps;
    struct tokenval tokval;
    expr *evalresult;
    enum pp_token_type needtype;
    const char *dname = pp_directives[ct];
    bool casesense = true;
    enum preproc_token cond = PP_COND(ct);

    origline = tline;

    switch (cond) {
    case PP_IFCTX:
        j = false;              /* have we matched yet? */
        while (true) {
            tline = skip_white(tline);
            if (!tline)
                break;
            if (tline->type != TOK_ID) {
                nasm_nonfatal("`%s' expects context identifiers",
                              dname);
                goto fail;
            }
            if (cstk && cstk->name && !nasm_stricmp(tok_text(tline), cstk->name))
                j = true;
            tline = tline->next;
        }
        break;

    case PP_IFDEF:
    case PP_IFDEFALIAS:
    {
        bool alias = cond == PP_IFDEFALIAS;
        SMacro *smac;
        Context *ctx;
        const char *mname;

        j = false;              /* have we matched yet? */
        while (tline) {
            tline = skip_white(tline);
            if (!tline || (tline->type != TOK_ID &&
			   tline->type != TOK_LOCAL_MACRO)) {
                nasm_nonfatal("`%s' expects macro identifiers",
                              dname);
                goto fail;
            }

            mname = tok_text(tline);
            ctx = get_ctx(mname, &mname);
            if (smacro_defined(ctx, mname, -1, &smac, true, alias) && smac
                && smac->alias == alias) {
                j = true;
                break;
            }
            tline = tline->next;
        }
        break;
    }

    case PP_IFENV:
        tline = expand_smacro(tline);
        j = false;              /* have we matched yet? */
        while (tline) {
            tline = skip_white(tline);
            if (!tline || (tline->type != TOK_ID &&
                           tline->type != TOK_STRING &&
			   tline->type != TOK_INTERNAL_STRING &&
                           tline->type != TOK_ENVIRON)) {
                nasm_nonfatal("`%s' expects environment variable names",
                              dname);
                goto fail;
            }

	    j |= !!pp_getenv(tline, false);
            tline = tline->next;
	}
	break;

    case PP_IFIDNI:
        casesense = false;
        /* fall through */
    case PP_IFIDN:
        tline = expand_smacro(tline);
        t = tt = tline;
        while (tok_isnt(tt, ','))
            tt = tt->next;
        if (!tt) {
            nasm_nonfatal("`%s' expects two comma-separated arguments",
                          dname);
            goto fail;
        }
        tt = tt->next;
        j = true;               /* assume equality unless proved not */
        while (tok_isnt(t, ',') && tt) {
	    unsigned int l1, l2;
	    const char *t1, *t2;

            if (tok_is(tt, ',')) {
                nasm_nonfatal("`%s': more than one comma on line",
                              dname);
                goto fail;
            }
            if (t->type == TOK_WHITESPACE) {
                t = t->next;
                continue;
            }
            if (tt->type == TOK_WHITESPACE) {
                tt = tt->next;
                continue;
            }
            if (tt->type != t->type) {
                j = false;      /* found mismatching tokens */
                break;
            }

	    t1 = unquote_token(t);
	    t2 = unquote_token(tt);
	    l1 = t->len;
	    l2 = tt->len;

	    if (l1 != l2 || mmemcmp(t1, t2, l1, casesense)) {
		j = false;
		break;
	    }

            t = t->next;
            tt = tt->next;
        }
        if (!tok_is(t, ',') || tt)
            j = false;          /* trailing gunk on one end or other */
        break;

    case PP_IFMACRO:
    {
        bool found = false;
        MMacro searching, *mmac;

        tline = skip_white(tline);
        tline = expand_id(tline);
        if (!tok_type(tline, TOK_ID)) {
            nasm_nonfatal("`%s' expects a macro name", dname);
            goto fail;
        }
        nasm_zero(searching);
        searching.name = dup_text(tline);
        searching.casesense = true;
        searching.nparam_min = 0;
        searching.nparam_max = INT_MAX;
        tline = expand_smacro(tline->next);
        tline = skip_white(tline);
        if (!tline) {
        } else if (!tok_type(tline, TOK_NUMBER)) {
            nasm_nonfatal("`%s' expects a parameter count or nothing",
                          dname);
        } else {
            searching.nparam_min = searching.nparam_max =
                read_param_count(tok_text(tline));
        }
        if (tline && tok_is(tline->next, '-')) {
            tline = tline->next->next;
            if (tok_is(tline, '*'))
                searching.nparam_max = INT_MAX;
            else if (!tok_type(tline, TOK_NUMBER))
                nasm_nonfatal("`%s' expects a parameter count after `-'",
                              dname);
            else {
                searching.nparam_max = read_param_count(tok_text(tline));
                if (searching.nparam_min > searching.nparam_max) {
                    nasm_nonfatal("minimum parameter count exceeds maximum");
                    searching.nparam_max = searching.nparam_min;
                }
            }
        }
        if (tline && tok_is(tline->next, '+')) {
            tline = tline->next;
            searching.plus = true;
        }
        mmac = (MMacro *) hash_findix(&mmacros, searching.name);
        while (mmac) {
            if (!strcmp(mmac->name, searching.name) &&
                (mmac->nparam_min <= searching.nparam_max
                 || searching.plus)
                && (searching.nparam_min <= mmac->nparam_max
                    || mmac->plus)) {
                found = true;
                break;
            }
            mmac = mmac->next;
        }
        if (tline && tline->next)
            nasm_warn(WARN_OTHER, "trailing garbage after %%ifmacro ignored");
        nasm_free(searching.name);
        j = found;
        break;
    }

    case PP_IFID:
        needtype = TOK_ID;
        goto iftype;
    case PP_IFNUM:
        needtype = TOK_NUMBER;
        goto iftype;
    case PP_IFSTR:
        needtype = TOK_STRING;
        goto iftype;

iftype:
        t = tline = expand_smacro(tline);

        while (tok_white(t) ||
               (needtype == TOK_NUMBER && (tok_is(t, '-') | tok_is(t, '+'))))
            t = t->next;

        j = tok_type(t, needtype);
        break;

    case PP_IFTOKEN:
        tline = expand_smacro(tline);
        t = skip_white(tline);

        j = false;
        if (t) {
            t = skip_white(t->next); /* Skip the actual token + whitespace */
            j = !t;
        }
        break;

    case PP_IFEMPTY:
        tline = expand_smacro(tline);
        t = skip_white(tline);
        j = !t;                 /* Should be empty */
        break;

    case PP_IF:
        pps.tptr = tline = expand_smacro(tline);
	pps.ntokens = -1;
        tokval.t_type = TOKEN_INVALID;
        evalresult = evaluate(ppscan, &pps, &tokval, NULL, true, NULL);
        if (!evalresult)
            return -1;
        if (tokval.t_type)
            nasm_warn(WARN_OTHER, "trailing garbage after expression ignored");
        if (!is_simple(evalresult)) {
            nasm_nonfatal("non-constant value given to `%s'",
                          dname);
            goto fail;
        }
        j = reloc_value(evalresult) != 0;
        break;

    case PP_IFUSING:
    case PP_IFUSABLE:
    {
        const struct use_package *pkg;
        const char *name;

        pkg = get_use_pkg(tline, dname, &name);
        if (!name)
            goto fail;

        j = pkg && ((cond == PP_IFUSABLE) | use_loaded[pkg->index]);
        break;
    }

    default:
        nasm_nonfatal("unknown preprocessor directive `%s'", dname);
        goto fail;
    }

    free_tlist(origline);
    return (j ^ PP_COND_NEGATIVE(ct)) ? COND_IF_TRUE : COND_IF_FALSE;

fail:
    free_tlist(origline);
    return COND_NEVER;
}