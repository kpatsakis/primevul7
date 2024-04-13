assert_inrange(typval_T *argvars)
{
    garray_T	ga;
    int		error = FALSE;
    char_u	*tofree;
    char	msg[200];
    char_u	numbuf[NUMBUFLEN];

#ifdef FEAT_FLOAT
    if (argvars[0].v_type == VAR_FLOAT
	    || argvars[1].v_type == VAR_FLOAT
	    || argvars[2].v_type == VAR_FLOAT)
    {
	float_T flower = tv_get_float(&argvars[0]);
	float_T fupper = tv_get_float(&argvars[1]);
	float_T factual = tv_get_float(&argvars[2]);

	if (factual < flower || factual > fupper)
	{
	    prepare_assert_error(&ga);
	    if (argvars[3].v_type != VAR_UNKNOWN)
	    {
		ga_concat(&ga, tv2string(&argvars[3], &tofree, numbuf, 0));
		vim_free(tofree);
	    }
	    else
	    {
		vim_snprintf(msg, 200, "Expected range %g - %g, but got %g",
						      flower, fupper, factual);
		ga_concat(&ga, (char_u *)msg);
	    }
	    assert_error(&ga);
	    ga_clear(&ga);
	    return 1;
	}
    }
    else
#endif
    {
	varnumber_T	lower = tv_get_number_chk(&argvars[0], &error);
	varnumber_T	upper = tv_get_number_chk(&argvars[1], &error);
	varnumber_T	actual = tv_get_number_chk(&argvars[2], &error);

	if (error)
	    return 0;
	if (actual < lower || actual > upper)
	{
	    prepare_assert_error(&ga);
	    if (argvars[3].v_type != VAR_UNKNOWN)
	    {
		ga_concat(&ga, tv2string(&argvars[3], &tofree, numbuf, 0));
		vim_free(tofree);
	    }
	    else
	    {
		vim_snprintf(msg, 200, "Expected range %ld - %ld, but got %ld",
				       (long)lower, (long)upper, (long)actual);
		ga_concat(&ga, (char_u *)msg);
	    }
	    assert_error(&ga);
	    ga_clear(&ga);
	    return 1;
	}
    }
    return 0;
}