static void mark_smac_params(Token *tline, const SMacro *tmpl,
                             enum pp_token_type type)
{
    const struct smac_param *params = tmpl->params;
    int nparam = tmpl->nparam;
    Token *t;
    int i;

    list_for_each(t, tline) {
        if (t->type != TOK_ID && t->type != TOK_XDEF_PARAM)
            continue;

        for (i = 0; i < nparam; i++) {
            if (tok_text_match(t, &params[i].name))
                t->type = type ? type : tok_smac_param(i);
        }
    }
}