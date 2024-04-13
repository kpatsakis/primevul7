onecap_copy(
    char_u	*word,
    char_u	*wcopy,
    int		upper)	    // TRUE: first letter made upper case
{
    char_u	*p;
    int		c;
    int		l;

    p = word;
    if (has_mbyte)
	c = mb_cptr2char_adv(&p);
    else
	c = *p++;
    if (upper)
	c = SPELL_TOUPPER(c);
    else
	c = SPELL_TOFOLD(c);
    if (has_mbyte)
	l = mb_char2bytes(c, wcopy);
    else
    {
	l = 1;
	wcopy[0] = c;
    }
    vim_strncpy(wcopy + l, p, MAXWLEN - l - 1);
}