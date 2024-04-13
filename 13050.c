f_test_ignore_error(typval_T *argvars, typval_T *rettv UNUSED)
{
    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    if (argvars[0].v_type != VAR_STRING)
	emsg(_(e_invalid_argument));
    else
	ignore_error_for_testing(tv_get_string(&argvars[0]));
}