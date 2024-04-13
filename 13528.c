swapchars(int op_type, pos_T *pos, int length)
{
    int todo;
    int	did_change = 0;

    for (todo = length; todo > 0; --todo)
    {
	if (has_mbyte)
	{
	    int len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    if (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	if (inc(pos) == -1)    // at end of file
	    break;
    }
    return did_change;
}