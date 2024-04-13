vim_unescape_csi(char_u *p)
{
    char_u	*s = p, *d = p;

    while (*s != NUL)
    {
	if (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	else if ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (int)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	else
	    *d++ = *s++;
    }
    *d = NUL;
}