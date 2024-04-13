fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    int		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = NULL;
    char_u	*p;

    if (name != NULL)
    {
	// Ignore case when 'fileignorecase' or the argument is set.
	rmp->rm_ic = p_fic || ignore_case;
	if (vim_regexec(rmp, name, (colnr_T)0))
	    match = name;
	else if (rmp->regprog != NULL)
	{
	    // Replace $(HOME) with '~' and try matching again.
	    p = home_replace_save(NULL, name);
	    if (p != NULL && vim_regexec(rmp, p, (colnr_T)0))
		match = name;
	    vim_free(p);
	}
    }

    return match;
}