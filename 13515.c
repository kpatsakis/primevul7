get_op_type(int char1, int char2)
{
    int		i;

    if (char1 == 'r')		// ignore second character
	return OP_REPLACE;
    if (char1 == '~')		// when tilde is an operator
	return OP_TILDE;
    if (char1 == 'g' && char2 == Ctrl_A)	// add
	return OP_NR_ADD;
    if (char1 == 'g' && char2 == Ctrl_X)	// subtract
	return OP_NR_SUB;
    if (char1 == 'z' && char2 == 'y')	// OP_YANK
	return OP_YANK;
    for (i = 0; ; ++i)
    {
	if (opchars[i][0] == char1 && opchars[i][1] == char2)
	    break;
	if (i == (int)ARRAY_LENGTH(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    break;
	}
    }
    return i;
}