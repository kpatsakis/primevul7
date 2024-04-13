static Token *new_Token_free(Token * next, enum pp_token_type type,
                             char *text, size_t txtlen)
{
    Token *t = alloc_Token();

    t->next = next;
    t->type = type;
    t->len = tok_check_len(txtlen);

    if (txtlen <= INLINE_TEXT) {
        memcpy(t->text.a, text, txtlen);
        free(text);
    } else {
        t->text.p.ptr = text;
    }

    return t;
}