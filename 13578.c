nfa_regcomp_start(
    char_u	*expr,
    int		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    int		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (int)(STRLEN(expr) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = sizeof(int) * nstate_max;

    post_start = alloc(postfix_size);
    if (post_start == NULL)
	return FAIL;
    post_ptr = post_start;
    post_end = post_start + nstate_max;
    wants_nfa = FALSE;
    rex.nfa_has_zend = FALSE;
    rex.nfa_has_backref = FALSE;

    // shared with BT engine
    regcomp_start(expr, re_flags);

    return OK;
}