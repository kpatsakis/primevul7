check_abbr(
    int		c,
    char_u	*ptr,
    int		col,
    int		mincol)
{
    int		len;
    int		scol;		/* starting column of the abbr. */
    int		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
#ifdef FEAT_LOCALMAP
    mapblock_T	*mp2;
#endif
    int		clen = 0;	/* length in characters */
    int		is_id = TRUE;
    int		vim_abbr;

    if (typebuf.tb_no_abbr_cnt)	/* abbrev. are not recursive */
	return FALSE;

    /* no remapping implies no abbreviation, except for CTRL-] */
    if ((KeyNoremap & (RM_NONE|RM_SCRIPT)) != 0 && c != Ctrl_RSB)
	return FALSE;

    /*
     * Check for word before the cursor: If it ends in a keyword char all
     * chars before it must be keyword chars or non-keyword chars, but not
     * white space. If it ends in a non-keyword char we accept any characters
     * before it except white space.
     */
    if (col == 0)				/* cannot be an abbr. */
	return FALSE;

    if (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	if (!vim_iswordp(p))
	    vim_abbr = TRUE;			/* Vim added abbr. */
	else
	{
	    vim_abbr = FALSE;			/* vi compatible abbr. */
	    if (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	while (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    if (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		break;
	    }
	    ++clen;
	}
	scol = (int)(p - ptr);
    }
    else
    {
	if (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			/* Vim added abbr. */
	else
	{
	    vim_abbr = FALSE;			/* vi compatible abbr. */
	    if (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	for (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    if (scol < mincol)
	scol = mincol;
    if (scol < col)		/* there is a word in front of the cursor */
    {
	ptr += scol;
	len = col - scol;
#ifdef FEAT_LOCALMAP
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	if (mp == NULL)
	{
	    mp = mp2;
	    mp2 = NULL;
	}
#else
	mp = first_abbr;
#endif
	for ( ; mp;
#ifdef FEAT_LOCALMAP
		mp->m_next == NULL ? (mp = mp2, mp2 = NULL) :
#endif
		(mp = mp->m_next))
	{
	    int		qlen = mp->m_keylen;
	    char_u	*q = mp->m_keys;
	    int		match;

	    if (vim_strbyte(mp->m_keys, K_SPECIAL) != NULL)
	    {
		char_u *qe = vim_strsave(mp->m_keys);

		/* might have CSI escaped mp->m_keys */
		if (qe != NULL)
		{
		    q = qe;
		    vim_unescape_csi(q);
		    qlen = (int)STRLEN(q);
		}
	    }

	    /* find entries with right mode and keys */
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && !STRNCMP(q, ptr, (size_t)len);
	    if (q != mp->m_keys)
		vim_free(q);
	    if (match)
		break;
	}
	if (mp != NULL)
	{
	    /*
	     * Found a match:
	     * Insert the rest of the abbreviation in typebuf.tb_buf[].
	     * This goes from end to start.
	     *
	     * Characters 0x000 - 0x100: normal chars, may need CTRL-V,
	     * except K_SPECIAL: Becomes K_SPECIAL KS_SPECIAL KE_FILLER
	     * Characters where IS_SPECIAL() == TRUE: key codes, need
	     * K_SPECIAL. Other characters (with ABBR_OFF): don't use CTRL-V.
	     *
	     * Character CTRL-] is treated specially - it completes the
	     * abbreviation, but is not inserted into the input stream.
	     */
	    j = 0;
	    if (c != Ctrl_RSB)
	    {
					/* special key code, split up */
		if (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		else
		{
		    if (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	/* special char needs CTRL-V */
		    if (has_mbyte)
		    {
			/* if ABBR_OFF has been added, remove it here */
			if (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			j += (*mb_char2bytes)(c, tb + j);
		    }
		    else
			tb[j++] = c;
		}
		tb[j] = NUL;
					/* insert the last typed char */
		(void)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }
#ifdef FEAT_EVAL
	    if (mp->m_expr)
		s = eval_map_expr(mp->m_str, c);
	    else
#endif
		s = mp->m_str;
	    if (s != NULL)
	    {
					/* insert the to string */
		(void)ins_typebuf(s, mp->m_noremap, 0, TRUE, mp->m_silent);
					/* no abbrev. for these chars */
		typebuf.tb_no_abbr_cnt += (int)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		if (mp->m_expr)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    if (has_mbyte)
		len = clen;	/* Delete characters instead of bytes */
	    while (len-- > 0)		/* delete the from string */
		(void)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    return TRUE;
	}
    }
    return FALSE;
}