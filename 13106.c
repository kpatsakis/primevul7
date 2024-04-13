regc(int b)
{
    if (regcode == JUST_CALC_SIZE)
	regsize++;
    else
	*regcode++ = b;
}