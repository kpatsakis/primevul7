static Token *quote_any_token(Token *t)
{
    size_t len = t->len;
    char *p;

    p = nasm_quote(tok_text(t), &len);
    t->type = TOK_STRING;
    return set_text_free(t, p, len);
}