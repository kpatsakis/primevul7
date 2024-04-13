test_gui_drop_files(dict_T *args UNUSED)
{
#  if defined(HAVE_DROP_FILE)
    int		row;
    int		col;
    int_u	mods;
    char_u	**fnames;
    int		count = 0;
    typval_T	t;
    list_T	*l;
    listitem_T	*li;

    if (!dict_has_key(args, "files")
	    || !dict_has_key(args, "row")
	    || !dict_has_key(args, "col")
	    || !dict_has_key(args, "modifiers"))
	return FALSE;

    (void)dict_get_tv(args, "files", &t);
    row = (int)dict_get_number(args, "row");
    col = (int)dict_get_number(args, "col");
    mods = (int)dict_get_number(args, "modifiers");

    if (t.v_type != VAR_LIST || list_len(t.vval.v_list) == 0)
	return FALSE;

    l = t.vval.v_list;
    fnames = ALLOC_MULT(char_u *, list_len(l));
    if (fnames == NULL)
	return FALSE;

    FOR_ALL_LIST_ITEMS(l, li)
    {
	// ignore non-string items
	if (li->li_tv.v_type != VAR_STRING
		|| li->li_tv.vval.v_string == NULL)
	    continue;

	fnames[count] = vim_strsave(li->li_tv.vval.v_string);
	if (fnames[count] == NULL)
	{
	    while (--count >= 0)
		vim_free(fnames[count]);
	    vim_free(fnames);
	    return FALSE;
	}
	count++;
    }

    if (count > 0)
	gui_handle_drop(TEXT_X(col - 1), TEXT_Y(row - 1), mods, fnames, count);
    else
	vim_free(fnames);
#  endif

    return TRUE;
}