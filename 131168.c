spell_soundfold_sofo(slang_T *slang, char_u *inword, char_u *res)
{
    char_u	*s;
    int		ri = 0;
    int		c;

    if (has_mbyte)
    {
	int	prevc = 0;
	int	*ip;

	// The sl_sal_first[] table contains the translation for chars up to
	// 255, sl_sal the rest.
	for (s = inword; *s != NUL; )
	{
	    c = mb_cptr2char_adv(&s);
	    if (enc_utf8 ? utf_class(c) == 0 : VIM_ISWHITE(c))
		c = ' ';
	    else if (c < 256)
		c = slang->sl_sal_first[c];
	    else
	    {
		ip = ((int **)slang->sl_sal.ga_data)[c & 0xff];
		if (ip == NULL)		// empty list, can't match
		    c = NUL;
		else
		    for (;;)		// find "c" in the list
		    {
			if (*ip == 0)	// not found
			{
			    c = NUL;
			    break;
			}
			if (*ip == c)	// match!
			{
			    c = ip[1];
			    break;
			}
			ip += 2;
		    }
	    }

	    if (c != NUL && c != prevc)
	    {
		ri += mb_char2bytes(c, res + ri);
		if (ri + MB_MAXBYTES > MAXWLEN)
		    break;
		prevc = c;
	    }
	}
    }
    else
    {
	// The sl_sal_first[] table contains the translation.
	for (s = inword; (c = *s) != NUL; ++s)
	{
	    if (VIM_ISWHITE(c))
		c = ' ';
	    else
		c = slang->sl_sal_first[c];
	    if (c != NUL && (ri == 0 || res[ri - 1] != c))
		res[ri++] = c;
	}
    }

    res[ri] = NUL;
}