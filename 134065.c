static void do_pragma_preproc(Token *tline)
{
    const char *txt;

    /* Skip to the real stuff */
    tline = tline->next;
    tline = skip_white(tline);

    if (!tok_type(tline, TOK_ID))
        return;

    txt = tok_text(tline);
    if (!nasm_stricmp(txt, "sane_empty_expansion")) {
        tline = skip_white(tline->next);
        ppopt.sane_empty_expansion =
            pp_get_boolean_option(tline, ppopt.sane_empty_expansion);
    } else {
        /* Unknown pragma, ignore for now */
    }
}