static Token *set_text(struct Token *t, const char *text, size_t len)
{
    char *textp;

    if (t->len > INLINE_TEXT)
	nasm_free(t->text.p.ptr);

    nasm_zero(t->text);

    t->len = len = tok_check_len(len);
    textp = (len > INLINE_TEXT)
	? (t->text.p.ptr = nasm_malloc(len+1)) : t->text.a;
    memcpy(textp, text, len);
    textp[len] = '\0';
    return t;
}