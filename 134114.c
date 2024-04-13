static int expand_mmacro(Token * tline)
{
    Token *startline = tline;
    Token *label = NULL;
    bool dont_prepend = false;
    Token **params, *t, *tt;
    MMacro *m;
    Line *l, *ll;
    int i, *paramlen;
    const char *mname;
    int nparam = 0;

    t = tline;
    t = skip_white(t);
    /*    if (!tok_type(t, TOK_ID))  Lino 02/25/02 */
    if (!tok_type(t, TOK_ID) && !tok_type(t, TOK_LOCAL_MACRO))
        return 0;
    m = is_mmacro(t, &nparam, &params);
    if (m) {
        mname = tok_text(t);
    } else {
        Token *last;
        /*
         * We have an id which isn't a macro call. We'll assume
         * it might be a label; we'll also check to see if a
         * colon follows it. Then, if there's another id after
         * that lot, we'll check it again for macro-hood.
         */
        label = last = t;
        t = t->next;
        if (tok_white(t))
            last = t, t = t->next;
        if (tok_is(t, ':')) {
            dont_prepend = true;
            last = t, t = t->next;
            if (tok_white(t))
                last = t, t = t->next;
        }
        if (!tok_type(t, TOK_ID) || !(m = is_mmacro(t, &nparam, &params)))
            return 0;
        last->next = NULL;
        mname = tok_text(t);
        tline = t;
    }

    if (unlikely(mmacro_deadman.total >= nasm_limit[LIMIT_MMACROS] ||
                 mmacro_deadman.levels >= nasm_limit[LIMIT_MACRO_LEVELS])) {
        if (!mmacro_deadman.triggered) {
            nasm_nonfatal("interminable multiline macro recursion");
            mmacro_deadman.triggered = true;
        }
        return 0;
    }

    mmacro_deadman.total++;
    mmacro_deadman.levels++;

    /*
     * Fix up the parameters: this involves stripping leading and
     * trailing whitespace and stripping braces if they are present.
     */
    nasm_newn(paramlen, nparam+1);

    for (i = 1; (t = params[i]); i++) {
        bool braced = false;
        int brace = 0;
        int white = 0;
        bool comma = !m->plus || i < nparam;

        t = skip_white(t);
        if (tok_is(t, '{')) {
            t = t->next;
            brace = 1;
            braced = true;
            comma = false;
        }

        params[i] = t;
        for (; t; t = t->next) {
            if (tok_white(t)) {
                white++;
                continue;
            }

            if (t->type == TOK_OTHER && t->len == 1) {
                switch (t->text.a[0]) {
                case ',':
                    if (comma && !brace)
                        goto endparam;
                    break;

                case '{':
                    brace++;
                    break;

                case '}':
                    brace--;
                    if (braced && !brace) {
                        paramlen[i] += white;
                        goto endparam;
                    }
                    break;

                default:
                    break;
                }
            }

            paramlen[i] += white + 1;
            white = 0;
        }
    endparam:
        ;
    }

    /*
     * OK, we have a MMacro structure together with a set of
     * parameters. We must now go through the expansion and push
     * copies of each Line on to istk->expansion. Substitution of
     * parameter tokens and macro-local tokens doesn't get done
     * until the single-line macro substitution process; this is
     * because delaying them allows us to change the semantics
     * later through %rotate and give the right semantics for
     * nested mmacros.
     *
     * First, push an end marker on to istk->expansion, mark this
     * macro as in progress, and set up its invocation-specific
     * variables.
     */
    nasm_new(ll);
    ll->next = istk->expansion;
    ll->finishes = m;
    ll->where = istk->where;
    istk->expansion = ll;

    /*
     * Save the previous MMacro expansion in the case of
     * macro recursion
     */
#if 0
    if (m->max_depth && m->in_progress)
        push_mmacro(m);
#endif

    m->in_progress ++;
    m->params = params;
    m->iline = tline;
    m->iname = nasm_strdup(mname);
    m->nparam = nparam;
    m->rotate = 0;
    m->paramlen = paramlen;
    m->unique = unique++;
    m->condcnt = 0;

    m->mstk = istk->mstk;
    istk->mstk.mstk = istk->mstk.mmac = m;

    list_for_each(l, m->expansion) {
        nasm_new(ll);
        ll->next = istk->expansion;
        istk->expansion = ll;
        ll->first = dup_tlist(l->first, NULL);
        ll->where = l->where;
    }

    /*
     * If we had a label, and this macro definition does not include
     * a %00, push it on as the first line of, ot
     * the macro expansion.
     */
    if (label) {
        /*
         * We had a label. If this macro contains an %00 parameter,
         * save the value as a special parameter (which is what it
         * is), otherwise push it as the first line of the macro
         * expansion.
         */
        if (m->capture_label) {
            params[0] = dup_Token(NULL, label);
            paramlen[0] = 1;
            free_tlist(startline);
       } else {
            nasm_new(ll);
            ll->finishes = NULL;
            ll->next = istk->expansion;
            istk->expansion = ll;
            ll->first = startline;
            ll->where = istk->where;
            if (!dont_prepend) {
                while (label->next)
                    label = label->next;
                label->next = tt = make_tok_char(NULL, ':');
            }
        }
    }

    istk->nolist += !!(m->nolist & NL_LIST);
    istk->noline += !!(m->nolist & NL_LINE);

    if (!istk->nolist) {
        lfmt->uplevel(LIST_MACRO, 0);

        if (list_option('m'))
            list_mmacro_call(m);
    }

    if (!istk->noline)
        src_macro_push(m, istk->where);

    return 1;
}