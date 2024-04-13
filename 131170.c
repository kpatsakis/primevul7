spell_iswordp_w(int *p, win_T *wp)
{
    int		*s;

    if (*p < 256 ? wp->w_s->b_spell_ismw[*p]
		 : (wp->w_s->b_spell_ismw_mb != NULL
			     && vim_strchr(wp->w_s->b_spell_ismw_mb, *p) != NULL))
	s = p + 1;
    else
	s = p;

    if (*s > 255)
    {
	if (enc_utf8)
	    return spell_mb_isword_class(utf_class(*s), wp);
	if (enc_dbcs)
	    return spell_mb_isword_class(
				dbcs_class((unsigned)*s >> 8, *s & 0xff), wp);
	return 0;
    }
    return spelltab.st_isw[*s];
}