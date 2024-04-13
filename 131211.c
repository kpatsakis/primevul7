spell_mb_isword_class(int cl, win_T *wp)
{
    if (wp->w_s->b_cjk)
	// East Asian characters are not considered word characters.
	return cl == 2 || cl == 0x2800;
    return cl >= 2 && cl != 0x2070 && cl != 0x2080 && cl != 3;
}