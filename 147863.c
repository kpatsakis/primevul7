test_gui_tabmenu_event(dict_T *args UNUSED)
{
#  ifdef FEAT_GUI_TABLINE
    int	tabnr;
    int	item;

    if (!dict_has_key(args, "tabnr")
	    || !dict_has_key(args, "item"))
	return FALSE;

    tabnr = (int)dict_get_number(args, "tabnr");
    item = (int)dict_get_number(args, "item");

    send_tabline_menu_event(tabnr, item);
#  endif
    return TRUE;
}