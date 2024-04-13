f_test_getvalue(typval_T *argvars, typval_T *rettv)
{
    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    if (argvars[0].v_type != VAR_STRING)
	emsg(_(e_invalid_argument));
    else
    {
	char_u *name = tv_get_string(&argvars[0]);

	if (STRCMP(name, (char_u *)"need_fileinfo") == 0)
	    rettv->vval.v_number = need_fileinfo;
	else
	    semsg(_(e_invalid_argument_str), name);
    }
}