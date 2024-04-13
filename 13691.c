cmd_exists(char_u *name)
{
    exarg_T	ea;
    int		full = FALSE;
    int		i;
    int		j;
    char_u	*p;

    // Check command modifiers.
    for (i = 0; i < (int)ARRAY_LENGTH(cmdmods); ++i)
    {
	for (j = 0; name[j] != NUL; ++j)
	    if (name[j] != cmdmods[i].name[j])
		break;
	if (name[j] == NUL && j >= cmdmods[i].minlen)
	    return (cmdmods[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, NULL, NULL);
    if (p == NULL)
	return 3;
    if (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	return 0;
    if (*skipwhite(p) != NUL)
	return 0;	// trailing garbage
    return (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}