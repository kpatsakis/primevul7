use_midword(slang_T *lp, win_T *wp)
{
    char_u	*p;

    if (lp->sl_midword == NULL)	    // there aren't any
	return;

    for (p = lp->sl_midword; *p != NUL; )
	if (has_mbyte)
	{
	    int	    c, l, n;
	    char_u  *bp;

	    c = mb_ptr2char(p);
	    l = (*mb_ptr2len)(p);
	    if (c < 256 && l <= 2)
		wp->w_s->b_spell_ismw[c] = TRUE;
	    else if (wp->w_s->b_spell_ismw_mb == NULL)
		// First multi-byte char in "b_spell_ismw_mb".
		wp->w_s->b_spell_ismw_mb = vim_strnsave(p, l);
	    else
	    {
		// Append multi-byte chars to "b_spell_ismw_mb".
		n = (int)STRLEN(wp->w_s->b_spell_ismw_mb);
		bp = vim_strnsave(wp->w_s->b_spell_ismw_mb, n + l);
		if (bp != NULL)
		{
		    vim_free(wp->w_s->b_spell_ismw_mb);
		    wp->w_s->b_spell_ismw_mb = bp;
		    vim_strncpy(bp + n, p, l);
		}
	    }
	    p += l;
	}
	else
	    wp->w_s->b_spell_ismw[*p++] = TRUE;
}