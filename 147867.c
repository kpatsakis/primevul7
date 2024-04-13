test_gui_tabline_event(dict_T *args UNUSED)
{
#  ifdef FEAT_GUI_TABLINE
    int		tabnr;

    if (!dict_has_key(args, "tabnr"))
	return FALSE;

    tabnr = (int)dict_get_number(args, "tabnr");

    return send_tabline_event(tabnr);
#  else
    return FALSE;
#  endif
}