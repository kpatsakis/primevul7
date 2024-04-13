regnext(char_u *p)
{
    int	    offset;

    if (p == JUST_CALC_SIZE || reg_toolong)
	return NULL;

    offset = NEXT(p);
    if (offset == 0)
	return NULL;

    if (OP(p) == BACK)
	return p - offset;
    else
	return p + offset;
}