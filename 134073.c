static Token *new_Token(Token * next, enum pp_token_type type,
                        const char *text, size_t txtlen)
{
    Token *t = alloc_Token();
    char *textp;

    t->next = next;
    t->type = type;
    if (type == TOK_WHITESPACE) {
        t->len = 1;
        t->text.a[0] = ' ';
    } else {
        if (text && text[0] && !txtlen)
            txtlen = tok_strlen(text);

        t->len = tok_check_len(txtlen);

        if (text) {
            textp = (txtlen > INLINE_TEXT)
                ? (t->text.p.ptr = nasm_malloc(txtlen+1)) : t->text.a;
            memcpy(textp, text, txtlen);
            textp[txtlen] = '\0';   /* In case we needed malloc() */
        } else {
            /*
             * Allocate a buffer but do not fill it. The caller
             * can fill in text, but must not change the length.
             * The filled in text must be exactly txtlen once
             * the buffer is filled and before the token is added
             * to any line lists.
             */
            if (txtlen > INLINE_TEXT)
                t->text.p.ptr = nasm_zalloc(txtlen+1);
        }
    }
    return t;
}