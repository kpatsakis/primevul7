static Token *expand_smacro_noreset(Token *org_tline)
{
    Token *tline;
    bool expanded;
    errhold errhold;       /* Hold warning/errors during expansion */

    if (!org_tline)
        return NULL;            /* Empty input */

    /*
     * Trick: we should avoid changing the start token pointer since it can
     * be contained in "next" field of other token. Because of this
     * we allocate a copy of first token and work with it; at the end of
     * routine we copy it back
     */
    tline = dup_Token(org_tline->next, org_tline);

    /*
     * Pretend that we always end up doing expansion on the first pass;
     * that way %+ get processed. However, if we process %+ before the
     * first pass, we end up with things like MACRO %+ TAIL trying to
     * look up the macro "MACROTAIL", which we don't want.
     */
    expanded = true;

    while (true) {
        static const struct tokseq_match tmatch[] = {
            {
                PP_CONCAT_MASK(TOK_ID)          |
		PP_CONCAT_MASK(TOK_LOCAL_MACRO) |
		PP_CONCAT_MASK(TOK_ENVIRON)     |
                PP_CONCAT_MASK(TOK_PREPROC_ID),     /* head */
                PP_CONCAT_MASK(TOK_ID)          |
		PP_CONCAT_MASK(TOK_LOCAL_MACRO) |
		PP_CONCAT_MASK(TOK_ENVIRON)     |
                PP_CONCAT_MASK(TOK_PREPROC_ID)  |
                PP_CONCAT_MASK(TOK_NUMBER)          /* tail */
            }
        };
        Token **tail = &tline;

        /*
         * We hold warnings/errors until we are done this this loop. It is
         * possible for nuisance warnings to appear that disappear on later
         * passes.
         */
        errhold = nasm_error_hold_push();

        while (*tail)           /* main token loop */
            expanded |= !!expand_one_smacro(&tail);

         if (!expanded)
            break;              /* Done! */

        /*
         * Now scan the entire line and look for successive TOK_IDs
         * that resulted after expansion (they can't be produced by
         * tokenize()). The successive TOK_IDs should be concatenated.
         * Also we look for %+ tokens and concatenate the tokens
         * before and after them (without white spaces in between).
         */
        if (!paste_tokens(&tline, tmatch, ARRAY_SIZE(tmatch), true))
            break;              /* Done again! */

        nasm_error_hold_pop(errhold, false);
        expanded = false;
    }
    nasm_error_hold_pop(errhold, true);

    if (!tline) {
        /*
         * The expression expanded to empty line;
         * we can't return NULL because of the "trick" above.
         * Just set the line to a single WHITESPACE token.
	 */

	tline = new_White(NULL);
    }

    steal_Token(org_tline, tline);
    org_tline->next = tline->next;
    delete_Token(tline);

    return org_tline;
}