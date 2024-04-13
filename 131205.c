byte_in_str(char_u *str, int n)
{
    char_u	*p;

    for (p = str; *p != NUL; ++p)
	if (*p == n)
	    return TRUE;
    return FALSE;
}