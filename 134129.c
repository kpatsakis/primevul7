static Token *set_text_free(struct Token *t, char *text, unsigned int len)
{
    char *textp;

    if (t->len > INLINE_TEXT)
	nasm_free(t->text.p.ptr);

    nasm_zero(t->text);

    t->len = len = tok_check_len(len);
    if (len > INLINE_TEXT) {
	textp = t->text.p.ptr = text;
    } else {
	textp = memcpy(t->text.a, text, len);
	nasm_free(text);
    }
    textp[len] = '\0';

    return t;
}