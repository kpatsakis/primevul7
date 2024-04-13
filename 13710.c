ex_resize(exarg_T *eap)
{
    int		n;
    win_T	*wp = curwin;

    if (eap->addr_count > 0)
    {
	n = eap->line2;
	for (wp = firstwin; wp->w_next != NULL && --n > 0; wp = wp->w_next)
	    ;
    }

# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif
    n = atol((char *)eap->arg);
    if (cmdmod.cmod_split & WSP_VERT)
    {
	if (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	else if (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    else
    {
	if (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	else if (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}