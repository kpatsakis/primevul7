did_set_spell_option(int is_spellfile)
{
    char    *errmsg = NULL;
    win_T   *wp;
    int	    l;

    if (is_spellfile)
    {
	l = (int)STRLEN(curwin->w_s->b_p_spf);
	if (l > 0 && (l < 4
			|| STRCMP(curwin->w_s->b_p_spf + l - 4, ".add") != 0))
	    errmsg = e_invalid_argument;
    }

    if (errmsg == NULL)
    {
	FOR_ALL_WINDOWS(wp)
	    if (wp->w_buffer == curbuf && wp->w_p_spell)
	    {
		errmsg = did_set_spelllang(wp);
		break;
	    }
    }
    return errmsg;
}