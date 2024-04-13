get_autoload_prefix(scriptitem_T *si)
{
    char_u *p = script_name_after_autoload(si);
    char_u *prefix;

    if (p == NULL)
	return NULL;
    prefix = vim_strsave(p);
    if (prefix == NULL)
	return NULL;

    // replace all '/' with '#' and locate ".vim" at the end
    for (p = prefix; *p != NUL; p += mb_ptr2len(p))
    {
	if (vim_ispathsep(*p))
	    *p = '#';
	else if (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    return prefix;
	}
    }

    // did not find ".vim" at the end
    vim_free(prefix);
    return NULL;
}