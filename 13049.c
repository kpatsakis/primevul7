f_test_option_not_set(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    if (argvars[0].v_type != VAR_STRING)
	emsg(_(e_invalid_argument));
    else
    {
	name = tv_get_string(&argvars[0]);
	if (reset_option_was_set(name) == FAIL)
	    semsg(_(e_invalid_argument_str), name);
    }
}