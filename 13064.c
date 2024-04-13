test_gui_find_repl(dict_T *args)
{
    int		flags;
    char_u	*find_text;
    char_u	*repl_text;
    int		forward;
    int		retval;

    if (dict_find(args, (char_u *)"find_text", -1) == NULL
	    || dict_find(args, (char_u *)"repl_text", -1) == NULL
	    || dict_find(args, (char_u *)"flags", -1) == NULL
	    || dict_find(args, (char_u *)"forward", -1) == NULL)
	return FALSE;

    find_text = dict_get_string(args, (char_u *)"find_text", TRUE);
    repl_text = dict_get_string(args, (char_u *)"repl_text", TRUE);
    flags = (int)dict_get_number(args, (char_u *)"flags");
    forward = (int)dict_get_number(args, (char_u *)"forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    return retval;
}