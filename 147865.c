test_gui_find_repl(dict_T *args)
{
    int		flags;
    char_u	*find_text;
    char_u	*repl_text;
    int		forward;
    int		retval;

    if (!dict_has_key(args, "find_text")
	    || !dict_has_key(args, "repl_text")
	    || !dict_has_key(args, "flags")
	    || !dict_has_key(args, "forward"))
	return FALSE;

    find_text = dict_get_string(args, "find_text", TRUE);
    repl_text = dict_get_string(args, "repl_text", TRUE);
    flags = (int)dict_get_number(args, "flags");
    forward = (int)dict_get_number(args, "forward");

    retval = gui_do_findrepl(flags, find_text, repl_text, forward);
    vim_free(find_text);
    vim_free(repl_text);

    return retval;
}