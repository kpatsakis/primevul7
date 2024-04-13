valid_spellfile(char_u *val)
{
    char_u *s;

    for (s = val; *s != NUL; ++s)
	if (!vim_isfilec(*s) && *s != ',' && *s != ' ')
	    return FALSE;
    return TRUE;
}