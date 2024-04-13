nfa_regbranch(void)
{
    int		old_post_pos;

    old_post_pos = (int)(post_ptr - post_start);

    // First branch, possibly the only one
    if (nfa_regconcat() == FAIL)
	return FAIL;

    // Try next concats
    while (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	if (old_post_pos == (int)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (int)(post_ptr - post_start);
	if (nfa_regconcat() == FAIL)
	    return FAIL;
	// if concat is empty do emit a node
	if (old_post_pos == (int)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    if (old_post_pos == (int)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    return OK;
}