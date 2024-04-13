eval_map_expr(
    char_u	*str,
    int		c)	    /* NUL or typed character for abbreviation */
{
    char_u	*res;
    char_u	*p;
    char_u	*expr;
    pos_T	save_cursor;
    int		save_msg_col;
    int		save_msg_row;

    /* Remove escaping of CSI, because "str" is in a format to be used as
     * typeahead. */
    expr = vim_strsave(str);
    if (expr == NULL)
	return NULL;
    vim_unescape_csi(expr);

    /* Forbid changing text or using ":normal" to avoid most of the bad side
     * effects.  Also restore the cursor position. */
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  /* set v:char to the typed character */
    save_cursor = curwin->w_cursor;
    save_msg_col = msg_col;
    save_msg_row = msg_row;
    p = eval_to_string(expr, NULL, FALSE);
    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    msg_col = save_msg_col;
    msg_row = save_msg_row;

    vim_free(expr);

    if (p == NULL)
	return NULL;
    /* Escape CSI in the result to be able to use the string as typeahead. */
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    return res;
}