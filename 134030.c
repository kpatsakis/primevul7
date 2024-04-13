static int find_cc(Token * t)
{
    Token *tt;

    if (!t)
        return -1;              /* Probably a %+ without a space */

    t = skip_white(t);
    if (!tok_type(t, TOK_ID))
        return -1;
    tt = t->next;
    tt = skip_white(tt);
    if (tok_isnt(tt, ','))
        return -1;

    return bsii(tok_text(t), (const char **)conditions,
		ARRAY_SIZE(conditions));
}