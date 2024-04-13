one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    if (in_vim9script())
	return FALSE;
    if (*p == 'k')
    {
	*idx = CMD_k;
	return TRUE;
    }
    if (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	return TRUE;
    }
    return FALSE;
}