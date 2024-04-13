static char *detoken(Token * tlist, bool expand_locals)
{
    Token *t;
    char *line, *p;
    int len = 0;

    list_for_each(t, tlist) {
	switch (t->type) {
	case TOK_ENVIRON:
	{
	    const char *v = pp_getenv(t, true);
	    set_text(t, v, tok_strlen(v));
	    t->type = TOK_NAKED_STRING;
	    break;
        }

	case TOK_LOCAL_MACRO:
        case TOK_LOCAL_SYMBOL:
	    if (expand_locals) {
		const char *q;
		char *p;
		Context *ctx = get_ctx(tok_text(t), &q);
		if (ctx) {
		    p = nasm_asprintf("..@%"PRIu64".%s", ctx->number, q);
		    set_text_free(t, p, nasm_last_string_len());
		    t->type = TOK_ID;
		}
	    }
	    break;

        case TOK_INDIRECT:
            /*
             * This won't happen in when emitting to the assembler,
             * but can happen when emitting output for some of the
             * list options. The token string doesn't actually include
             * the brackets in this case.
             */
            len += 3;           /* %[] */
            break;

	default:
	    break;		/* No modifications */
        }

        if (debug_level(2)) {
            unsigned int t_len  = t->len;
            unsigned int s_len = tok_strlen(tok_text(t));
            if (t_len != s_len) {
                nasm_panic("assertion failed: token \"%s\" type %u len %u has t->len %u\n",
                           tok_text(t), t->type, s_len, t_len);
                t->len = s_len;
            }
        }

	len += t->len;
    }

    p = line = nasm_malloc(len + 1);

    list_for_each(t, tlist) {
        switch (t->type) {
        case TOK_INDIRECT:
            *p++ = '%';
            *p++ = '[';
            p = mempcpy(p, tok_text(t), t->len);
            *p++ = ']';
            break;

        default:
            p = mempcpy(p, tok_text(t), t->len);
        }
    }
    *p = '\0';

    return line;
}