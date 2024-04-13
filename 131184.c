spell_cat_line(char_u *buf, char_u *line, int maxlen)
{
    char_u	*p;
    int		n;

    p = skipwhite(line);
    while (vim_strchr((char_u *)"*#/\"\t", *p) != NULL)
	p = skipwhite(p + 1);

    if (*p != NUL)
    {
	// Only worth concatenating if there is something else than spaces to
	// concatenate.
	n = (int)(p - line) + 1;
	if (n < maxlen - 1)
	{
	    vim_memset(buf, ' ', n);
	    vim_strncpy(buf +  n, p, maxlen - 1 - n);
	}
    }
}