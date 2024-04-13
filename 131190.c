dump_word(
    slang_T	*slang,
    char_u	*word,
    char_u	*pat,
    int		*dir,
    int		dumpflags,
    int		wordflags,
    linenr_T	lnum)
{
    int		keepcap = FALSE;
    char_u	*p;
    char_u	*tw;
    char_u	cword[MAXWLEN];
    char_u	badword[MAXWLEN + 10];
    int		i;
    int		flags = wordflags;

    if (dumpflags & DUMPFLAG_ONECAP)
	flags |= WF_ONECAP;
    if (dumpflags & DUMPFLAG_ALLCAP)
	flags |= WF_ALLCAP;

    if ((dumpflags & DUMPFLAG_KEEPCASE) == 0 && (flags & WF_CAPMASK) != 0)
    {
	// Need to fix case according to "flags".
	make_case_word(word, cword, flags);
	p = cword;
    }
    else
    {
	p = word;
	if ((dumpflags & DUMPFLAG_KEEPCASE)
		&& ((captype(word, NULL) & WF_KEEPCAP) == 0
						 || (flags & WF_FIXCAP) != 0))
	    keepcap = TRUE;
    }
    tw = p;

    if (pat == NULL)
    {
	// Add flags and regions after a slash.
	if ((flags & (WF_BANNED | WF_RARE | WF_REGION)) || keepcap)
	{
	    STRCPY(badword, p);
	    STRCAT(badword, "/");
	    if (keepcap)
		STRCAT(badword, "=");
	    if (flags & WF_BANNED)
		STRCAT(badword, "!");
	    else if (flags & WF_RARE)
		STRCAT(badword, "?");
	    if (flags & WF_REGION)
		for (i = 0; i < 7; ++i)
		    if (flags & (0x10000 << i))
			sprintf((char *)badword + STRLEN(badword), "%d", i + 1);
	    p = badword;
	}

	if (dumpflags & DUMPFLAG_COUNT)
	{
	    hashitem_T  *hi;

	    // Include the word count for ":spelldump!".
	    hi = hash_find(&slang->sl_wordcount, tw);
	    if (!HASHITEM_EMPTY(hi))
	    {
		vim_snprintf((char *)IObuff, IOSIZE, "%s\t%d",
						     tw, HI2WC(hi)->wc_count);
		p = IObuff;
	    }
	}

	ml_append(lnum, p, (colnr_T)0, FALSE);
    }
    else if (((dumpflags & DUMPFLAG_ICASE)
		    ? MB_STRNICMP(p, pat, STRLEN(pat)) == 0
		    : STRNCMP(p, pat, STRLEN(pat)) == 0)
		&& ins_compl_add_infercase(p, (int)STRLEN(p),
					  p_ic, NULL, *dir, FALSE) == OK)
	// if dir was BACKWARD then honor it just once
	*dir = FORWARD;
}