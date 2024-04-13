regnode(int op)
{
    char_u  *ret;

    ret = regcode;
    if (ret == JUST_CALC_SIZE)
	regsize += 3;
    else
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    return ret;
}