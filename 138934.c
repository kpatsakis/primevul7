makemap(
    FILE	*fd,
    buf_T	*buf)	    /* buffer for local mappings or NULL */
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    char	*cmd;
    int		abbr;
    int		hash;
    int		did_cpo = FALSE;
    int		i;

    validate_maphash();

    /*
     * Do the loop twice: Once for mappings, once for abbreviations.
     * Then loop over all map hash lists.
     */
    for (abbr = 0; abbr < 2; ++abbr)
	for (hash = 0; hash < 256; ++hash)
	{
	    if (abbr)
	    {
		if (hash > 0)		/* there is only one abbr list */
		    break;
#ifdef FEAT_LOCALMAP
		if (buf != NULL)
		    mp = buf->b_first_abbr;
		else
#endif
		    mp = first_abbr;
	    }
	    else
	    {
#ifdef FEAT_LOCALMAP
		if (buf != NULL)
		    mp = buf->b_maphash[hash];
		else
#endif
		    mp = maphash[hash];
	    }

	    for ( ; mp; mp = mp->m_next)
	    {
		/* skip script-local mappings */
		if (mp->m_noremap == REMAP_SCRIPT)
		    continue;

		/* skip mappings that contain a <SNR> (script-local thing),
		 * they probably don't work when loaded again */
		for (p = mp->m_str; *p != NUL; ++p)
		    if (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (int)KE_SNR)
			break;
		if (*p != NUL)
		    continue;

		/* It's possible to create a mapping and then ":unmap" certain
		 * modes.  We recreate this here by mapping the individual
		 * modes, which requires up to three of them. */
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		if (abbr)
		    cmd = "abbr";
		else
		    cmd = "map";
		switch (mp->m_mode)
		{
		    case NORMAL + VISUAL + SELECTMODE + OP_PENDING:
			break;
		    case NORMAL:
			c1 = 'n';
			break;
		    case VISUAL:
			c1 = 'x';
			break;
		    case SELECTMODE:
			c1 = 's';
			break;
		    case OP_PENDING:
			c1 = 'o';
			break;
		    case NORMAL + VISUAL:
			c1 = 'n';
			c2 = 'x';
			break;
		    case NORMAL + SELECTMODE:
			c1 = 'n';
			c2 = 's';
			break;
		    case NORMAL + OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			break;
		    case VISUAL + SELECTMODE:
			c1 = 'v';
			break;
		    case VISUAL + OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			break;
		    case SELECTMODE + OP_PENDING:
			c1 = 's';
			c2 = 'o';
			break;
		    case NORMAL + VISUAL + SELECTMODE:
			c1 = 'n';
			c2 = 'v';
			break;
		    case NORMAL + VISUAL + OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			break;
		    case NORMAL + SELECTMODE + OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			break;
		    case VISUAL + SELECTMODE + OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			break;
		    case CMDLINE + INSERT:
			if (!abbr)
			    cmd = "map!";
			break;
		    case CMDLINE:
			c1 = 'c';
			break;
		    case INSERT:
			c1 = 'i';
			break;
		    case LANGMAP:
			c1 = 'l';
			break;
		    case TERMINAL:
			c1 = 't';
			break;
		    default:
			iemsg(_("E228: makemap: Illegal mode"));
			return FAIL;
		}
		do	/* do this twice if c2 is set, 3 times with c3 */
		{
		    /* When outputting <> form, need to make sure that 'cpo'
		     * is set to the Vim default. */
		    if (!did_cpo)
		    {
			if (*mp->m_str == NUL)		/* will use <Nop> */
			    did_cpo = TRUE;
			else
			    for (i = 0; i < 2; ++i)
				for (p = (i ? mp->m_str : mp->m_keys); *p; ++p)
				    if (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			if (did_cpo)
			{
			    if (fprintf(fd, "let s:cpo_save=&cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				return FAIL;
			}
		    }
		    if (c1 && putc(c1, fd) < 0)
			return FAIL;
		    if (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			return FAIL;
		    if (fputs(cmd, fd) < 0)
			return FAIL;
		    if (buf != NULL && fputs(" <buffer>", fd) < 0)
			return FAIL;
		    if (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			return FAIL;
		    if (mp->m_silent && fputs(" <silent>", fd) < 0)
			return FAIL;
#ifdef FEAT_EVAL
		    if (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			return FAIL;
		    if (mp->m_expr && fputs(" <expr>", fd) < 0)
			return FAIL;
#endif

		    if (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_keys, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			return FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} while (c1 != NUL);
	    }
	}

    if (did_cpo)
	if (fprintf(fd, "let &cpo=s:cpo_save") < 0
		|| put_eol(fd) < 0
		|| fprintf(fd, "unlet s:cpo_save") < 0
		|| put_eol(fd) < 0)
	    return FAIL;
    return OK;
}