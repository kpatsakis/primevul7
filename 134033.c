static Token *dup_Token(Token *next, const Token *src)
{
    Token *t = alloc_Token();

    memcpy(t, src, sizeof *src);
    t->next = next;

    if (t->len > INLINE_TEXT) {
        t->text.p.ptr = nasm_malloc(t->len + 1);
        memcpy(t->text.p.ptr, src->text.p.ptr, t->len+1);
    }

    return t;
}