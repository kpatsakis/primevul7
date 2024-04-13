static inline const char *tok_text(const struct Token *t)
{
    return (t->len <= INLINE_TEXT) ? t->text.a : t->text.p.ptr;
}