test_gui_mouse_event(dict_T *args)
{
    int		button;
    int		row;
    int		col;
    int		repeated_click;
    int_u	mods;
    int		move;

    if (!dict_has_key(args, "row")
	    || !dict_has_key(args, "col"))
	return FALSE;

    // Note: "move" is optional, requires fewer arguments
    move = (int)dict_get_bool(args, "move", FALSE);

    if (!move && (!dict_has_key(args, "button")
	    || !dict_has_key(args, "multiclick")
	    || !dict_has_key(args, "modifiers")))
	return FALSE;

    row = (int)dict_get_number(args, "row");
    col = (int)dict_get_number(args, "col");

    if (move)
    {
	if (dict_get_bool(args, "cell", FALSE))
	{
	    // click in the middle of the character cell
	    row = row * gui.char_height + gui.char_height / 2;
	    col = col * gui.char_width + gui.char_width / 2;
	}
	gui_mouse_moved(col, row);
    }
    else
    {
	button = (int)dict_get_number(args, "button");
	repeated_click = (int)dict_get_number(args, "multiclick");
	mods = (int)dict_get_number(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }

    return TRUE;
}