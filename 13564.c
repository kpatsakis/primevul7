addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    int			*ip)
{
    int tlen = l->n;
    int count;
    int listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate().
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET);
    if (r == NULL)
	return NULL;

    // when "*ip" was at the end of the list, nothing to do
    if (listidx + 1 == tlen)
	return r;

    // re-order to put the new state at the current position
    count = l->n - tlen;
    if (count == 0)
	return r; // no state got added
    if (count == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    else if (count > 1)
    {
	if (l->n + count - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    int		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * sizeof(nfa_thread_T);
	    nfa_thread_T    *newl;

	    if ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_maxmempat));
		return NULL;
	    }
	    newl = alloc(newsize);
	    if (newl == NULL)
		return NULL;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    sizeof(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - count]),
		    sizeof(nfa_thread_T) * count);
	    mch_memmove(&(newl[listidx + count]),
		    &(l->t[listidx + 1]),
		    sizeof(nfa_thread_T) * (l->n - count - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	else
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + count]),
		    &(l->t[listidx + 1]),
		    sizeof(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    sizeof(nfa_thread_T) * count);
	}
    }
    --l->n;
    *ip = listidx - 1;

    return r;
}