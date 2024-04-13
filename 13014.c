assert_match_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    int		called_emsg_before = called_emsg;
    char_u	*pat;
    char_u	*text;

    if (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	return 1;

    pat = tv_get_string_buf_chk(&argvars[0], buf1);
    text = tv_get_string_buf_chk(&argvars[1], buf2);
    if (called_emsg == called_emsg_before
		 && pattern_match(pat, text, FALSE) != (atype == ASSERT_MATCH))
    {
	prepare_assert_error(&ga);
	fill_assert_error(&ga, &argvars[2], NULL, &argvars[0], &argvars[1],
									atype);
	assert_error(&ga);
	ga_clear(&ga);
	return 1;
    }
    return 0;
}