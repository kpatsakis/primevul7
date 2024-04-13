spell_casefold(
    win_T	*wp,
    char_u	*str,
    int		len,
    char_u	*buf,
    int		buflen)
{
    int		i;

    if (len >= buflen)
    {
	buf[0] = NUL;
	return FAIL;		// result will not fit
    }

    if (has_mbyte)
    {
	int	outi = 0;
	char_u	*p;
	int	c;

	// Fold one character at a time.
	for (p = str; p < str + len; )
	{
	    if (outi + MB_MAXBYTES > buflen)
	    {
		buf[outi] = NUL;
		return FAIL;
	    }
	    c = mb_cptr2char_adv(&p);

	    // Exception: greek capital sigma 0x03A3 folds to 0x03C3, except
	    // when it is the last character in a word, then it folds to
	    // 0x03C2.
	    if (c == 0x03a3 || c == 0x03c2)
	    {
		if (p == str + len || !spell_iswordp(p, wp))
		    c = 0x03c2;
		else
		    c = 0x03c3;
	    }
	    else
		c = SPELL_TOFOLD(c);

	    outi += mb_char2bytes(c, buf + outi);
	}
	buf[outi] = NUL;
    }
    else
    {
	// Be quick for non-multibyte encodings.
	for (i = 0; i < len; ++i)
	    buf[i] = spelltab.st_fold[str[i]];
	buf[i] = NUL;
    }

    return OK;
}