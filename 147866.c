test_gui_scrollbar(dict_T *args)
{
    char_u	*which;
    long	value;
    int		dragging;
    scrollbar_T *sb = NULL;

    if (!dict_has_key(args, "which")
	    || !dict_has_key(args, "value")
	    || !dict_has_key(args, "dragging"))
	return FALSE;

    which = dict_get_string(args, "which", FALSE);
    value = (long)dict_get_number(args, "value");
    dragging = (int)dict_get_number(args, "dragging");

    if (STRCMP(which, "left") == 0)
	sb = &curwin->w_scrollbars[SBAR_LEFT];
    else if (STRCMP(which, "right") == 0)
	sb = &curwin->w_scrollbars[SBAR_RIGHT];
    else if (STRCMP(which, "hor") == 0)
	sb = &gui.bottom_sbar;
    if (sb == NULL)
    {
	semsg(_(e_invalid_argument_str), which);
	return FALSE;
    }
    gui_drag_scrollbar(sb, value, dragging);
#  ifndef USE_ON_FLY_SCROLL
    // need to loop through normal_cmd() to handle the scroll events
    exec_normal(FALSE, TRUE, FALSE);
#  endif

    return TRUE;
}