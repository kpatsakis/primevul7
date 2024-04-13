test_gui_tabmenu_event(dict_T *args UNUSED)
{
#  ifdef FEAT_GUI_TABLINE
    int	tabnr;
    int	item;

    if (dict_find(args, (char_u *)"tabnr", -1) == NULL
	    || dict_find(args, (char_u *)"item", -1) == NULL)
	return FALSE;

    tabnr = (int)dict_get_number(args, (char_u *)"tabnr");
    item = (int)dict_get_number(args, (char_u *)"item");

    send_tabline_menu_event(tabnr, item);
#  endif
    return TRUE;
}