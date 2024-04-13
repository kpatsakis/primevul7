f_test_settime(typval_T *argvars, typval_T *rettv UNUSED)
{
    if (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	return;

    time_for_testing = (time_t)tv_get_number(&argvars[0]);
}