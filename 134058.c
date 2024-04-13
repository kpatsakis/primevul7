static bool pp_get_boolean_option(Token *tline, bool defval)
{
    static const char * const noyes[] = {
        "no", "yes",
        "false", "true",
        "off", "on"
    };
    struct ppscan pps;
    struct tokenval tokval;
    expr *evalresult;

    tline = skip_white(tline);
    if (!tline)
        return true;

    if (tline->type == TOK_ID) {
        size_t i;
	const char *txt = tok_text(tline);

        for (i = 0; i < ARRAY_SIZE(noyes); i++)
            if (!nasm_stricmp(txt, noyes[i]))
                return i & 1;
    }

    pps.tptr = NULL;
    pps.tptr = tline;
    pps.ntokens = -1;
    tokval.t_type = TOKEN_INVALID;
    evalresult = evaluate(ppscan, &pps, &tokval, NULL, true, NULL);

    if (!evalresult)
        return true;

    if (tokval.t_type)
        nasm_warn(WARN_OTHER, "trailing garbage after expression ignored");
    if (!is_really_simple(evalresult)) {
        nasm_nonfatal("boolean flag expression must be a constant");
        return defval;
    }

    return reloc_value(evalresult) != 0;
}