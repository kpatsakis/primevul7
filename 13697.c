checkforcmd_opt(
    char_u	**pp,		// start of command
    char	*cmd,		// name of command
    int		len,		// required length
    int		noparen)
{
    int		i;

    for (i = 0; cmd[i] != NUL; ++i)
	if (((char_u *)cmd)[i] != (*pp)[i])
	    break;
    if (i >= len && !isalpha((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	return TRUE;
    }
    return FALSE;
}