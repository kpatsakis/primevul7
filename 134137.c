list_smacro_def(enum preproc_token op, const Context *ctx, const SMacro *m)
{
    Token *t;
    size_t namelen, size;
    char *def, *p;
    char *context_prefix = NULL;
    size_t context_len;

    namelen = strlen(m->name);
    size = namelen + 2;  /* Include room for space after name + NUL */

    if (ctx) {
        int context_depth = cstk->depth - ctx->depth + 1;
        context_prefix =
            nasm_asprintf("[%s::%"PRIu64"] %%%-*s",
                          ctx->name ? ctx->name : "",
                          ctx->number, context_depth, "");

        context_len = nasm_last_string_len();
        memset(context_prefix + context_len - context_depth,
               '$', context_depth);
        size += context_len;
    }

    list_for_each(t, m->expansion)
        size += t->len;

    if (m->nparam) {
        /*
         * Space for ( and either , or ) around each
         * parameter, plus up to 4 flags.
         */
        int i;

        size += 1 + 4 * m->nparam;
        for (i = 0; i < m->nparam; i++)
            size += m->params[i].name.len;
    }

    def = nasm_malloc(size);
    p = def+size;
    *--p = '\0';

    list_for_each(t, m->expansion) {
	p -= t->len;
	memcpy(p, tok_text(t), t->len);
    }

    *--p = ' ';

    if (m->nparam) {
        int i;

        *--p = ')';
        for (i = m->nparam-1; i >= 0; i--) {
            enum sparmflags flags = m->params[i].flags;
            if (flags & SPARM_GREEDY)
                *--p = '+';
	    p -= m->params[i].name.len;
	    memcpy(p, tok_text(&m->params[i].name), m->params[i].name.len);

            if (flags & SPARM_NOSTRIP)
                *--p = '!';
            if (flags & SPARM_STR)
                *--p = '&';
            if (flags & SPARM_EVAL)
                *--p = '=';
            *--p = ',';
        }
        *p = '(';               /* First parameter starts with ( not , */
    }

    p -= namelen;
    memcpy(p, m->name, namelen);

    if (context_prefix) {
        p -= context_len;
        memcpy(p, context_prefix, context_len);
        nasm_free(context_prefix);
    }

    nasm_listmsg("%s %s", pp_directives[op], p);
    nasm_free(def);
}