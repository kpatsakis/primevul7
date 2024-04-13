assert_equal_common(typval_T *argvars, assert_type_T atype)
{
    garray_T	ga;

    if (tv_equal(&argvars[0], &argvars[1], FALSE, FALSE)
						   != (atype == ASSERT_EQUAL))
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