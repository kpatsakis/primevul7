ExpandRTDir(
    char_u	*pat,
    int		flags,
    int		*num_file,
    char_u	***file,
    char	*dirnames[])
{
    char_u	*s;
    char_u	*e;
    char_u	*match;
    garray_T	ga;
    int		i;
    int		pat_len;

    *num_file = 0;
    *file = NULL;
    pat_len = (int)STRLEN(pat);
    ga_init2(&ga, sizeof(char *), 10);

    for (i = 0; dirnames[i] != NULL; ++i)
    {
	s = alloc(STRLEN(dirnames[i]) + pat_len + 7);
	if (s == NULL)
	{
	    ga_clear_strings(&ga);
	    return FAIL;
	}
	sprintf((char *)s, "%s/%s*.vim", dirnames[i], pat);
	globpath(p_rtp, s, &ga, 0);
	vim_free(s);
    }

    if (flags & DIP_START) {
	for (i = 0; dirnames[i] != NULL; ++i)
	{
	    s = alloc(STRLEN(dirnames[i]) + pat_len + 22);
	    if (s == NULL)
	    {
		ga_clear_strings(&ga);
		return FAIL;
	    }
	    sprintf((char *)s, "pack/*/start/*/%s/%s*.vim", dirnames[i], pat);
	    globpath(p_pp, s, &ga, 0);
	    vim_free(s);
	}
    }

    if (flags & DIP_OPT) {
	for (i = 0; dirnames[i] != NULL; ++i)
	{
	    s = alloc(STRLEN(dirnames[i]) + pat_len + 20);
	    if (s == NULL)
	    {
		ga_clear_strings(&ga);
		return FAIL;
	    }
	    sprintf((char *)s, "pack/*/opt/*/%s/%s*.vim", dirnames[i], pat);
	    globpath(p_pp, s, &ga, 0);
	    vim_free(s);
	}
    }

    for (i = 0; i < ga.ga_len; ++i)
    {
	match = ((char_u **)ga.ga_data)[i];
	s = match;
	e = s + STRLEN(s);
	if (e - 4 > s && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    for (s = e; s > match; MB_PTR_BACK(match, s))
		if (s < match || vim_ispathsep(*s))
		    break;
	    ++s;
	    *e = NUL;
	    mch_memmove(match, s, e - s + 1);
	}
    }

    if (ga.ga_len == 0)
	return FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_data;
    *num_file = ga.ga_len;
    return OK;
}