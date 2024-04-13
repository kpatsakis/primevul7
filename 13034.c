test_gui_tabline_event(dict_T *args UNUSED)
{
#  ifdef FEAT_GUI_TABLINE
    int		tabnr;

    if (dict_find(args, (char_u *)"tabnr", -1) == NULL)
	return FALSE;

    tabnr = (int)dict_get_number(args, (char_u *)"tabnr");

    return send_tabline_event(tabnr);
#  else
    return FALSE;
#  endif
}