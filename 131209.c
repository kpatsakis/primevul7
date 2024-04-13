nofold_len(char_u *fword, int flen, char_u *word)
{
    char_u	*p;
    int		i = 0;

    for (p = fword; p < fword + flen; MB_PTR_ADV(p))
	++i;
    for (p = word; i > 0; MB_PTR_ADV(p))
	--i;
    return (int)(p - word);
}