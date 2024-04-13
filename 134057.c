static bool paste_tokens(Token **head, const struct tokseq_match *m,
                         size_t mnum, bool handle_explicit)
{
    Token *tok, *t, *next, **prev_next, **prev_nonspace, **nextp;
    bool pasted = false;
    char *buf, *p;
    size_t len, i;

    /*
     * The last token before pasting. We need it
     * to be able to connect new handled tokens.
     * In other words if there were a tokens stream
     *
     * A -> B -> C -> D
     *
     * and we've joined tokens B and C, the resulting
     * stream should be
     *
     * A -> BC -> D
     */
    tok = *head;
    prev_next = prev_nonspace = head;

    if (tok_white(tok) || tok_type(tok, TOK_PASTE))
        prev_nonspace = NULL;

    while (tok && (next = tok->next)) {
        bool did_paste = false;

        switch (tok->type) {
        case TOK_WHITESPACE:
            /* Zap redundant whitespaces */
            tok->next = next = zap_white(next);
            break;

        case TOK_PASTE:
            /* Explicit pasting */
            if (!handle_explicit)
                break;

            did_paste = true;

            /* Left pasting token is start of line, just drop %+ */
            if (!prev_nonspace) {
                prev_next = nextp = head;
                t = NULL;
            } else {
                prev_next = prev_nonspace;
                t = *prev_next;
                nextp = &t->next;
            }

            /*
             * Delete the %+ token itself plus any whitespace.
             * In a sequence of %+ ... %+ ... %+ pasting sequences where
             * some expansions in the middle have ended up empty,
             * we can end up having multiple %+ tokens in a row;
             * just drop whem in that case.
             */
            while ((next = *nextp)) {
                if (next->type == TOK_PASTE || next->type == TOK_WHITESPACE)
                    *nextp = delete_Token(next);
                else
                    break;
            }

            /*
             * Nothing after? Just leave the existing token.
             */
            if (!next)
                break;

            if (!t) {
                /* Nothing to actually paste, just zapping the paste */
                *prev_next = tok = next;
                break;
            }

            /* An actual paste */
            p = buf = nasm_malloc(t->len + next->len + 1);
            p = mempcpy(p, tok_text(t), t->len);
            p = mempcpy(p, tok_text(next), next->len);
            *p = '\0';
            delete_Token(t);
            t = tokenize(buf);
            nasm_free(buf);

            if (unlikely(!t)) {
                /*
                 * No output at all? Replace with a single whitespace.
                 * This should never happen.
                 */
                tok = t = new_White(NULL);
            } else {
                *prev_nonspace = tok = t;
            }
            while (t->next)
                t = t->next;    /* Find the last token produced */

            /* Delete the second token and attach to the end of the list */
            t->next = delete_Token(next);

            /* We want to restart from the head of the pasted token */
            *prev_next = next = tok;
            break;

        default:
            /* implicit pasting */
            for (i = 0; i < mnum; i++) {
                if (pp_concat_match(tok, m[i].mask_head))
                    break;
            }

            if (i >= mnum)
                break;

            len =  tok->len;
            while (pp_concat_match(next, m[i].mask_tail)) {
                len += next->len;
                next = next->next;
            }

            /* No match or no text to process */
            if (len == tok->len)
                break;

            p = buf = nasm_malloc(len + 1);
            while (tok != next) {
                p = mempcpy(p, tok_text(tok), tok->len);
                tok = delete_Token(tok);
            }
            *p = '\0';
            *prev_next = tok = t = tokenize(buf);
            nasm_free(buf);

            /*
             * Connect pasted into original stream,
             * ie A -> new-tokens -> B
             */
            while ((tok = t->next)) {
                if (tok->type != TOK_WHITESPACE && tok->type != TOK_PASTE)
                    prev_nonspace = &t->next;
                t = tok;
            }

            t->next = next;
            prev_next = &t->next;
            did_paste = true;
            break;
        }

        if (did_paste) {
            pasted = true;
        } else {
            prev_next = &tok->next;
            if (next && next->type != TOK_WHITESPACE && next->type != TOK_PASTE)
                prev_nonspace = prev_next;
        }

        tok = next;
    }

    return pasted;
}