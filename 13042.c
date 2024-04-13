test_gui_mouse_event(dict_T *args)
{
    int		button;
    int		row;
    int		col;
    int		repeated_click;
    int_u	mods;

    if (dict_find(args, (char_u *)"button", -1) == NULL
	    || dict_find(args, (char_u *)"row", -1) == NULL
	    || dict_find(args, (char_u *)"col", -1) == NULL
	    || dict_find(args, (char_u *)"multiclick", -1) == NULL
	    || dict_find(args, (char_u *)"modifiers", -1) == NULL)
	return FALSE;

    button = (int)dict_get_number(args, (char_u *)"button");
    row = (int)dict_get_number(args, (char_u *)"row");
    col = (int)dict_get_number(args, (char_u *)"col");
    repeated_click = (int)dict_get_number(args, (char_u *)"multiclick");
    mods = (int)dict_get_number(args, (char_u *)"modifiers");

    gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    return TRUE;
}