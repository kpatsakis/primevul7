f_assert_report(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    prepare_assert_error(&ga);
    ga_concat(&ga, tv_get_string(&argvars[0]));
    assert_error(&ga);
    ga_clear(&ga);
    rettv->vval.v_number = 1;
}