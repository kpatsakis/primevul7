swapchar(int op_type, pos_T *pos)
{
    int	    c;
    int	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    if (c >= 0x80 && op_type == OP_ROT13)
	return FALSE;

    if (op_type == OP_UPPER && c == 0xdf
		      && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling of German sharp s: change to "SS".
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char('S');
	ins_char('S');
	curwin->w_cursor = sp;
	inc(pos);
    }

    if (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	return FALSE;
    nc = c;
    if (MB_ISLOWER(c))
    {
	if (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	else if (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    else if (MB_ISUPPER(c))
    {
	if (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	else if (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    if (nc != c)
    {
	if (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_bytes(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	else
	    PBYTE(*pos, nc);
	return TRUE;
    }
    return FALSE;
}