STATIC void
S_skip_to_be_ignored_text(pTHX_ RExC_state_t *pRExC_state,
                                char ** p,
                                const bool force_to_xmod
                         )
{
    /* If the text at the current parse position '*p' is a '(?#...)' comment,
     * or if we are under /x or 'force_to_xmod' is TRUE, and the text at '*p'
     * is /x whitespace, advance '*p' so that on exit it points to the first
     * byte past all such white space and comments */

    const bool use_xmod = force_to_xmod || (RExC_flags & RXf_PMf_EXTENDED);

    PERL_ARGS_ASSERT_SKIP_TO_BE_IGNORED_TEXT;

    assert( ! UTF || UTF8_IS_INVARIANT(**p) || UTF8_IS_START(**p));

    for (;;) {
	if (RExC_end - (*p) >= 3
	    && *(*p)     == '('
	    && *(*p + 1) == '?'
	    && *(*p + 2) == '#')
	{
	    while (*(*p) != ')') {
		if ((*p) == RExC_end)
		    FAIL("Sequence (?#... not terminated");
		(*p)++;
	    }
	    (*p)++;
	    continue;
	}

	if (use_xmod) {
            const char * save_p = *p;
            while ((*p) < RExC_end) {
                STRLEN len;
                if ((len = is_PATWS_safe((*p), RExC_end, UTF))) {
                    (*p) += len;
                }
                else if (*(*p) == '#') {
                    (*p) = reg_skipcomment(pRExC_state, (*p));
                }
                else {
                    break;
                }
            }
            if (*p != save_p) {
                continue;
            }
	}

        break;
    }

    return;