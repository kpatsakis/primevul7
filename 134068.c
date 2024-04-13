static inline char *tok_text_buf(struct Token *t)
{
    return (t->len <= INLINE_TEXT) ? t->text.a : t->text.p.ptr;
}