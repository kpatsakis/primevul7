spell_iswordp_nmw(char_u *p, win_T *wp)
{
    int		c;

    if (has_mbyte)
    {
	c = mb_ptr2char(p);
	if (c > 255)
	    return spell_mb_isword_class(mb_get_class(p), wp);
	return spelltab.st_isw[c];
    }
    return spelltab.st_isw[*p];
}