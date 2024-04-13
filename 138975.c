vim_strsave_escape_csi(
    char_u *p)
{
    char_u	*res;
    char_u	*s, *d;

    /* Need a buffer to hold up to three times as much.  Four in case of an
     * illegal utf-8 byte:
     * 0xc0 -> 0xc3 0x80 -> 0xc3 K_SPECIAL KS_SPECIAL KE_FILLER */
    res = alloc((unsigned)(STRLEN(p) * 4) + 1);
    if (res != NULL)
    {
	d = res;
	for (s = p; *s != NUL; )
	{
	    if (s[0] == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	    {
		/* Copy special key unmodified. */
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	    }
	    else
	    {
		/* Add character, possibly multi-byte to destination, escaping
		 * CSI and K_SPECIAL. Be careful, it can be an illegal byte! */
		d = add_char2buf(PTR2CHAR(s), d);
		s += MB_CPTR2LEN(s);
	    }
	}
	*d = NUL;
    }
    return res;
}