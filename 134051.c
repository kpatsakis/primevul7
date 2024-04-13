static const char *get_id(Token **tp, const char *dname)
{
    const char *id;
    Token *t = *tp;

    t = t->next;                /* Skip directive */
    t = skip_white(t);
    t = expand_id(t);

    if (!is_macro_id(t)) {
        nasm_nonfatal("`%s' expects a macro identifier", dname);
        return NULL;
    }

    id = tok_text(t);
    t = skip_white(t);
    *tp = t;
    return id;
}