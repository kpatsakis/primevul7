regmbc(int c)
{
    if (!has_mbyte && c > 0xff)
	return;
    if (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    else
	regcode += (*mb_char2bytes)(c, regcode);
}