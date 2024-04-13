f_test_feedinput(typval_T *argvars, typval_T *rettv UNUSED)
{
#ifdef USE_INPUT_BUF
    char_u	*val;

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    val = tv_get_string_chk(&argvars[0]);
# ifdef VIMDLL
    // this doesn't work in the console
    if (!gui.in_use)
	return;
# endif

    if (val != NULL)
    {
	trash_input_buf();
	add_to_input_buf_csi(val, (int)STRLEN(val));
    }
#endif
}