static void list_mmacro_call(const MMacro *m)
{
    const char prefix[] = " ;;; [macro] ";
    size_t namelen, size;
    char *buf, *p;
    unsigned int i;
    const Token *t;

    namelen = strlen(m->iname);
    size = namelen + sizeof(prefix); /* Includes final null (from prefix) */

    for (i = 1; i <= m->nparam; i++) {
        int j = 0;
        size += 3;              /* Braces and space/comma */
        list_for_each(t, m->params[i]) {
            if (j++ >= m->paramlen[i])
                break;
            size += (t->type == TOK_WHITESPACE) ? 1 : t->len;
        }
    }

    buf = p = nasm_malloc(size);
    p = mempcpy(p, prefix, sizeof(prefix) - 1);
    p = mempcpy(p, m->iname, namelen);
    *p++ = ' ';

    for (i = 1; i <= m->nparam; i++) {
        int j = 0;
        *p++ = '{';
        list_for_each(t, m->params[i]) {
            if (j++ >= m->paramlen[i])
                break;
	    p = mempcpy(p, tok_text(t), t->len);
        }
        *p++ = '}';
        *p++ = ',';
    }

    *--p = '\0';                /* Replace last delimeter with null */
    lfmt->line(LIST_MACRO, -1, buf);
    nasm_free(buf);
}