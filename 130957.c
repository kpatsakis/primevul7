do_ascii(exarg_T *eap UNUSED)
{
    int		c;
    int		cval;
    char	buf1[20];
    char	buf2[20];
    char_u	buf3[7];
#ifdef FEAT_DIGRAPHS
    char_u      *dig;
#endif
    int		cc[MAX_MCO];
    int		ci = 0;
    int		len;

    if (enc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), cc);
    else
	c = gchar_cursor();
    if (c == NUL)
    {
	msg("NUL");
	return;
    }

    IObuff[0] = NUL;
    if (!has_mbyte || (enc_dbcs != 0 && c < 0x100) || c < 0x80)
    {
	if (c == NL)	    // NUL is stored as NL
	    c = NUL;
	if (c == CAR && get_fileformat(curbuf) == EOL_MAC)
	    cval = NL;	    // NL is stored as CR
	else
	    cval = c;
	if (vim_isprintc_strict(c) && (c < ' ' || c > '~'))
	{
	    transchar_nonprint(curbuf, buf3, c);
	    vim_snprintf(buf1, sizeof(buf1), "  <%s>", (char *)buf3);
	}
	else
	    buf1[0] = NUL;
	if (c >= 0x80)
	    vim_snprintf(buf2, sizeof(buf2), "  <M-%s>",
						 (char *)transchar(c & 0x7f));
	else
	    buf2[0] = NUL;
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(cval);
	if (dig != NULL)
	    vim_snprintf((char *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Oct %03o, Digr %s"),
			      transchar(c), buf1, buf2, cval, cval, cval, dig);
	else
#endif
	    vim_snprintf((char *)IObuff, IOSIZE,
		_("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
				  transchar(c), buf1, buf2, cval, cval, cval);
	if (enc_utf8)
	    c = cc[ci++];
	else
	    c = 0;
    }

    // Repeat for combining characters.
    while (has_mbyte && (c >= 0x100 || (enc_utf8 && c >= 0x80)))
    {
	len = (int)STRLEN(IObuff);
	// This assumes every multi-byte char is printable...
	if (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	if (enc_utf8 && utf_iscomposing(c)
#ifdef USE_GUI
		&& !gui.in_use
#endif
		)
	    IObuff[len++] = ' '; // draw composing char on top of a space
	len += (*mb_char2bytes)(c, IObuff + len);
#ifdef FEAT_DIGRAPHS
	dig = get_digraph_for_char(c);
	if (dig != NULL)
	    vim_snprintf((char *)IObuff + len, IOSIZE - len,
			c < 0x10000 ? _("> %d, Hex %04x, Oct %o, Digr %s")
				    : _("> %d, Hex %08x, Oct %o, Digr %s"),
					c, c, c, dig);
	else
#endif
	    vim_snprintf((char *)IObuff + len, IOSIZE - len,
			 c < 0x10000 ? _("> %d, Hex %04x, Octal %o")
				     : _("> %d, Hex %08x, Octal %o"),
				     c, c, c);
	if (ci == MAX_MCO)
	    break;
	if (enc_utf8)
	    c = cc[ci++];
	else
	    c = 0;
    }

    msg((char *)IObuff);
}