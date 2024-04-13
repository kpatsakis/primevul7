spell_check(
    win_T	*wp,		// current window
    char_u	*ptr,
    hlf_T	*attrp,
    int		*capcol,	// column to check for Capital
    int		docount)	// count good words
{
    matchinf_T	mi;		// Most things are put in "mi" so that it can
				// be passed to functions quickly.
    int		nrlen = 0;	// found a number first
    int		c;
    int		wrongcaplen = 0;
    int		lpi;
    int		count_word = docount;
    int		use_camel_case = *wp->w_s->b_p_spo != NUL;
    int		camel_case = 0;

    // A word never starts at a space or a control character.  Return quickly
    // then, skipping over the character.
    if (*ptr <= ' ')
	return 1;

    // Return here when loading language files failed.
    if (wp->w_s->b_langp.ga_len == 0)
	return 1;

    CLEAR_FIELD(mi);

    // A number is always OK.  Also skip hexadecimal numbers 0xFF99 and
    // 0X99FF.  But always do check spelling to find "3GPP" and "11
    // julifeest".
    if (*ptr >= '0' && *ptr <= '9')
    {
	if (*ptr == '0' && (ptr[1] == 'b' || ptr[1] == 'B'))
	    mi.mi_end = skipbin(ptr + 2);
	else if (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
	    mi.mi_end = skiphex(ptr + 2);
	else
	    mi.mi_end = skipdigits(ptr);
	nrlen = (int)(mi.mi_end - ptr);
    }

    // Find the normal end of the word (until the next non-word character).
    mi.mi_word = ptr;
    mi.mi_fend = ptr;
    if (spell_iswordp(mi.mi_fend, wp))
    {
	int prev_upper;
	int this_upper = FALSE;  // init for gcc

	if (use_camel_case)
	{
	    c = PTR2CHAR(mi.mi_fend);
	    this_upper = SPELL_ISUPPER(c);
	}

	do
	{
	    MB_PTR_ADV(mi.mi_fend);
	    if (use_camel_case)
	    {
		prev_upper = this_upper;
		c = PTR2CHAR(mi.mi_fend);
		this_upper = SPELL_ISUPPER(c);
		camel_case = !prev_upper && this_upper;
	    }
	} while (*mi.mi_fend != NUL && spell_iswordp(mi.mi_fend, wp)
							       && !camel_case);

	if (capcol != NULL && *capcol == 0 && wp->w_s->b_cap_prog != NULL)
	{
	    // Check word starting with capital letter.
	    c = PTR2CHAR(ptr);
	    if (!SPELL_ISUPPER(c))
		wrongcaplen = (int)(mi.mi_fend - ptr);
	}
    }
    if (capcol != NULL)
	*capcol = -1;

    // We always use the characters up to the next non-word character,
    // also for bad words.
    mi.mi_end = mi.mi_fend;

    // Check caps type later.
    mi.mi_capflags = 0;
    mi.mi_cend = NULL;
    mi.mi_win = wp;

    // case-fold the word with one non-word character, so that we can check
    // for the word end.
    if (*mi.mi_fend != NUL)
	MB_PTR_ADV(mi.mi_fend);

    (void)spell_casefold(wp, ptr, (int)(mi.mi_fend - ptr), mi.mi_fword,
							     MAXWLEN + 1);
    mi.mi_fwordlen = (int)STRLEN(mi.mi_fword);

    if (camel_case && mi.mi_fwordlen > 0)
	// Introduce a fake word end space into the folded word.
	mi.mi_fword[mi.mi_fwordlen - 1] = ' ';

    // The word is bad unless we recognize it.
    mi.mi_result = SP_BAD;
    mi.mi_result2 = SP_BAD;

    /*
     * Loop over the languages specified in 'spelllang'.
     * We check them all, because a word may be matched longer in another
     * language.
     */
    for (lpi = 0; lpi < wp->w_s->b_langp.ga_len; ++lpi)
    {
	mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, lpi);

	// If reloading fails the language is still in the list but everything
	// has been cleared.
	if (mi.mi_lp->lp_slang->sl_fidxs == NULL)
	    continue;

	// Check for a matching word in case-folded words.
	find_word(&mi, FIND_FOLDWORD);

	// Check for a matching word in keep-case words.
	find_word(&mi, FIND_KEEPWORD);

	// Check for matching prefixes.
	find_prefix(&mi, FIND_FOLDWORD);

	// For a NOBREAK language, may want to use a word without a following
	// word as a backup.
	if (mi.mi_lp->lp_slang->sl_nobreak && mi.mi_result == SP_BAD
						   && mi.mi_result2 != SP_BAD)
	{
	    mi.mi_result = mi.mi_result2;
	    mi.mi_end = mi.mi_end2;
	}

	// Count the word in the first language where it's found to be OK.
	if (count_word && mi.mi_result == SP_OK)
	{
	    count_common_word(mi.mi_lp->lp_slang, ptr,
						   (int)(mi.mi_end - ptr), 1);
	    count_word = FALSE;
	}
    }

    if (mi.mi_result != SP_OK)
    {
	// If we found a number skip over it.  Allows for "42nd".  Do flag
	// rare and local words, e.g., "3GPP".
	if (nrlen > 0)
	{
	    if (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
		return nrlen;
	}

	// When we are at a non-word character there is no error, just
	// skip over the character (try looking for a word after it).
	else if (!spell_iswordp_nmw(ptr, wp))
	{
	    if (capcol != NULL && wp->w_s->b_cap_prog != NULL)
	    {
		regmatch_T	regmatch;
		int		r;

		// Check for end of sentence.
		regmatch.regprog = wp->w_s->b_cap_prog;
		regmatch.rm_ic = FALSE;
		r = vim_regexec(&regmatch, ptr, 0);
		wp->w_s->b_cap_prog = regmatch.regprog;
		if (r)
		    *capcol = (int)(regmatch.endp[0] - ptr);
	    }

	    if (has_mbyte)
		return (*mb_ptr2len)(ptr);
	    return 1;
	}
	else if (mi.mi_end == ptr)
	    // Always include at least one character.  Required for when there
	    // is a mixup in "midword".
	    MB_PTR_ADV(mi.mi_end);
	else if (mi.mi_result == SP_BAD
		&& LANGP_ENTRY(wp->w_s->b_langp, 0)->lp_slang->sl_nobreak)
	{
	    char_u	*p, *fp;
	    int		save_result = mi.mi_result;

	    // First language in 'spelllang' is NOBREAK.  Find first position
	    // at which any word would be valid.
	    mi.mi_lp = LANGP_ENTRY(wp->w_s->b_langp, 0);
	    if (mi.mi_lp->lp_slang->sl_fidxs != NULL)
	    {
		p = mi.mi_word;
		fp = mi.mi_fword;
		for (;;)
		{
		    MB_PTR_ADV(p);
		    MB_PTR_ADV(fp);
		    if (p >= mi.mi_end)
			break;
		    mi.mi_compoff = (int)(fp - mi.mi_fword);
		    find_word(&mi, FIND_COMPOUND);
		    if (mi.mi_result != SP_BAD)
		    {
			mi.mi_end = p;
			break;
		    }
		}
		mi.mi_result = save_result;
	    }
	}

	if (mi.mi_result == SP_BAD || mi.mi_result == SP_BANNED)
	    *attrp = HLF_SPB;
	else if (mi.mi_result == SP_RARE)
	    *attrp = HLF_SPR;
	else
	    *attrp = HLF_SPL;
    }

    if (wrongcaplen > 0 && (mi.mi_result == SP_OK || mi.mi_result == SP_RARE))
    {
	// Report SpellCap only when the word isn't badly spelled.
	*attrp = HLF_SPC;
	return wrongcaplen;
    }

    return (int)(mi.mi_end - ptr);
}