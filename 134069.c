static const char *pp_getenv(const Token *t, bool warn)
{
    const char *txt = tok_text(t);
    const char *v;
    char *buf = NULL;
    bool is_string = false;

    if (!t)
	return NULL;

    switch (t->type) {
    case TOK_ENVIRON:
	txt += 2;		/* Skip leading %! */
	is_string = nasm_isquote(*txt);
	break;

    case TOK_STRING:
	is_string = true;
	break;

    case TOK_INTERNAL_STRING:
    case TOK_NAKED_STRING:
    case TOK_ID:
	is_string = false;
	break;

    default:
	return NULL;
    }

    if (is_string) {
	buf = nasm_strdup(txt);
	nasm_unquote_cstr(buf, NULL);
	txt = buf;
    }

    v = getenv(txt);
    if (warn && !v) {
	/*!
	 *!environment [on] nonexistent environment variable
	 *!  warns if a nonexistent environment variable
	 *!  is accessed using the \c{%!} preprocessor
	 *!  construct (see \k{getenv}.)  Such environment
	 *!  variables are treated as empty (with this
	 *!  warning issued) starting in NASM 2.15;
	 *!  earlier versions of NASM would treat this as
	 *!  an error.
	 */
	nasm_warn(WARN_ENVIRONMENT, "nonexistent environment variable `%s'", txt);
	v = "";
    }

    if (buf)
	nasm_free(buf);

    return v;
}