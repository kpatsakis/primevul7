no_spell_checking(win_T *wp)
{
    if (!wp->w_p_spell || *wp->w_s->b_p_spl == NUL
					 || wp->w_s->b_langp.ga_len == 0)
    {
	emsg(_(e_spell_checking_is_not_possible));
	return TRUE;
    }
    return FALSE;
}