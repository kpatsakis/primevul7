assert_beeps(typval_T *argvars, int no_beep)
{
    char_u	*cmd;
    garray_T	ga;
    int		ret = 0;

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return 0;

    cmd = tv_get_string_chk(&argvars[0]);
    called_vim_beep = FALSE;
    suppress_errthrow = TRUE;
    emsg_silent = FALSE;
    do_cmdline_cmd(cmd);
    if (no_beep ? called_vim_beep : !called_vim_beep)
    {
	prepare_assert_error(&ga);
	if (no_beep)
	    ga_concat(&ga, (char_u *)"command did beep: ");
	else
	    ga_concat(&ga, (char_u *)"command did not beep: ");
	ga_concat(&ga, cmd);
	assert_error(&ga);
	ga_clear(&ga);
	ret = 1;
    }

    suppress_errthrow = FALSE;
    emsg_on_display = FALSE;
    return ret;
}