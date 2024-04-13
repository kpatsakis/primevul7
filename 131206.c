allcap_copy(char_u *word, char_u *wcopy)
{
    char_u	*s;
    char_u	*d;
    int		c;

    d = wcopy;
    for (s = word; *s != NUL; )
    {
	if (has_mbyte)
	    c = mb_cptr2char_adv(&s);
	else
	    c = *s++;

	// We only change 0xdf to SS when we are certain latin1 is used.  It
	// would cause weird errors in other 8-bit encodings.
	if (enc_latin1like && c == 0xdf)
	{
	    c = 'S';
	    if (d - wcopy >= MAXWLEN - 1)
		break;
	    *d++ = c;
	}
	else
	    c = SPELL_TOUPPER(c);

	if (has_mbyte)
	{
	    if (d - wcopy >= MAXWLEN - MB_MAXBYTES)
		break;
	    d += mb_char2bytes(c, d);
	}
	else
	{
	    if (d - wcopy >= MAXWLEN - 1)
		break;
	    *d++ = c;
	}
    }
    *d = NUL;
}