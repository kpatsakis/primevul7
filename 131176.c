spell_iswordp(
    char_u	*p,
    win_T	*wp)	    // buffer used
{
    char_u	*s;
    int		l;
    int		c;

    if (has_mbyte)
    {
	l = mb_ptr2len(p);
	s = p;
	if (l == 1)
	{
	    // be quick for ASCII
	    if (wp->w_s->b_spell_ismw[*p])
		s = p + 1;		// skip a mid-word character
	}
	else
	{
	    c = mb_ptr2char(p);
	    if (c < 256 ? wp->w_s->b_spell_ismw[c]
		    : (wp->w_s->b_spell_ismw_mb != NULL
			   && vim_strchr(wp->w_s->b_spell_ismw_mb, c) != NULL))
		s = p + l;
	}

	c = mb_ptr2char(s);
	if (c > 255)
	    return spell_mb_isword_class(mb_get_class(s), wp);
	return spelltab.st_isw[c];
    }

    return spelltab.st_isw[wp->w_s->b_spell_ismw[*p] ? p[1] : p[0]];
}