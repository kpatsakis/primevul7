reginsert_nr(int op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    if (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	return;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    while (src > opnd)
	*--dst = *--src;

    place = opnd;		// Op node, where operand used to be.
    *place++ = op;
    *place++ = NUL;
    *place++ = NUL;
    re_put_long(place, (long_u)val);
}