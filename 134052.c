static const char *unquote_token_cstr(Token *t)
{
    if (t->type != TOK_STRING)
	return tok_text(t);

    t->type = TOK_INTERNAL_STRING;

    if (t->len > INLINE_TEXT) {
	char *p = t->text.p.ptr;

	t->len = nasm_unquote_cstr(p, NULL);

	if (t->len <= INLINE_TEXT) {
	    nasm_zero(t->text.a);
	    memcpy(t->text.a, p, t->len);
	    nasm_free(p);
	    return t->text.a;
	} else {
	    return p;
	}
    } else {
	t->len = nasm_unquote_cstr(t->text.a, NULL);
	return t->text.a;
    }
}