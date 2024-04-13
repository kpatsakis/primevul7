static void expand_mmac_params_range(MMacro *mac, Token *tline, Token ***tail)
{
    Token *t;
    const char *arg = tok_text(tline) + 1;
    int fst, lst, incr, n;
    int parsed;

    parsed = sscanf(arg, "%d:%d", &fst, &lst);
    nasm_assert(parsed == 2);

    /*
     * only macros params are accounted so
     * if someone passes %0 -- we reject such
     * value(s)
     */
    if (lst == 0 || fst == 0)
        goto err;

    /* the values should be sane */
    if ((fst > (int)mac->nparam || fst < (-(int)mac->nparam)) ||
        (lst > (int)mac->nparam || lst < (-(int)mac->nparam)))
        goto err;

    fst = fst < 0 ? fst + (int)mac->nparam + 1: fst;
    lst = lst < 0 ? lst + (int)mac->nparam + 1: lst;

    /*
     * It will be at least one parameter, as we can loop
     * in either direction.
     */
    incr = (fst < lst) ? 1 : -1;

    while (true) {
        n = mmac_rotate(mac, fst);
        dup_tlistn(mac->params[n], mac->paramlen[n], tail);
        if (fst == lst)
            break;
        t = make_tok_char(NULL, ',');
        **tail = t;
        *tail = &t->next;
        fst += incr;
    }

    return;

err:
    nasm_nonfatal("`%%{%s}': macro parameters out of range", arg);
    return;
}