f_assert_inrange(typval_T *argvars, typval_T *rettv)
{
    if (check_for_float_or_nr_arg(argvars, 0) == FAIL
	    || check_for_float_or_nr_arg(argvars, 1) == FAIL
	    || check_for_float_or_nr_arg(argvars, 2) == FAIL
	    || check_for_opt_string_arg(argvars, 3) == FAIL)
	return;

    rettv->vval.v_number = assert_inrange(argvars);
}