addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    int			off_arg)    // byte offset, when -1 go to next line
{
    int			subidx;
    int			off = off_arg;
    int			add_here = FALSE;
    int			listindex = 0;
    int			k;
    int			found = FALSE;
    nfa_thread_T	*thread;
    struct multipos	save_multipos;
    int			save_in_use;
    char_u		*save_ptr;
    int			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    static regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    int			did_print = FALSE;
#endif
    static int		depth = 0;

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    if (++depth >= 5000 || subs == NULL)
    {
	--depth;
	return NULL;
    }

    if (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    switch (state->c)
    {
	case NFA_NCLOSE:
	case NFA_MCLOSE:
	case NFA_MCLOSE1:
	case NFA_MCLOSE2:
	case NFA_MCLOSE3:
	case NFA_MCLOSE4:
	case NFA_MCLOSE5:
	case NFA_MCLOSE6:
	case NFA_MCLOSE7:
	case NFA_MCLOSE8:
	case NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	case NFA_ZCLOSE:
	case NFA_ZCLOSE1:
	case NFA_ZCLOSE2:
	case NFA_ZCLOSE3:
	case NFA_ZCLOSE4:
	case NFA_ZCLOSE5:
	case NFA_ZCLOSE6:
	case NFA_ZCLOSE7:
	case NFA_ZCLOSE8:
	case NFA_ZCLOSE9:
#endif
	case NFA_MOPEN:
	case NFA_ZEND:
	case NFA_SPLIT:
	case NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    break;

	case NFA_BOL:
	case NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    if (rex.input > rex.line
		    && *rex.input != NUL
		    && (nfa_endp == NULL
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		goto skip_add;
	    // FALLTHROUGH

	case NFA_MOPEN1:
	case NFA_MOPEN2:
	case NFA_MOPEN3:
	case NFA_MOPEN4:
	case NFA_MOPEN5:
	case NFA_MOPEN6:
	case NFA_MOPEN7:
	case NFA_MOPEN8:
	case NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	case NFA_ZOPEN:
	case NFA_ZOPEN1:
	case NFA_ZOPEN2:
	case NFA_ZOPEN3:
	case NFA_ZOPEN4:
	case NFA_ZOPEN5:
	case NFA_ZOPEN6:
	case NFA_ZOPEN7:
	case NFA_ZOPEN8:
	case NFA_ZOPEN9:
#endif
	case NFA_NOPEN:
	case NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	default:
	    if (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		if (!rex.nfa_has_backref && pim == NULL && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    if (add_here)
		    {
			for (k = 0; k < l->n && k < listindex; ++k)
			    if (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				break;
			    }
		    }
		    if (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == NULL ? "NULL" : "yes", l->has_pim, found);
#endif
			--depth;
			return subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		if (has_state_with_pos(l, state, subs, pim))
		    goto skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    if (l->n == l->len)
	    {
		int		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * sizeof(nfa_thread_T);
		nfa_thread_T	*newt;

		if ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_maxmempat));
		    --depth;
		    return NULL;
		}
		if (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    if (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		if (newt == NULL)
		{
		    // out of memory
		    --depth;
		    return NULL;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    if (pim == NULL)
		thread->pim.result = NFA_PIM_UNUSED;
	    else
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    if (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    if (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    switch (state->c)
    {
	case NFA_MATCH:
	    break;

	case NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg);
	    subs = addstate(l, state->out1, subs, pim, off_arg);
	    break;

	case NFA_EMPTY:
	case NFA_NOPEN:
	case NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg);
	    break;

	case NFA_MOPEN:
	case NFA_MOPEN1:
	case NFA_MOPEN2:
	case NFA_MOPEN3:
	case NFA_MOPEN4:
	case NFA_MOPEN5:
	case NFA_MOPEN6:
	case NFA_MOPEN7:
	case NFA_MOPEN8:
	case NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	case NFA_ZOPEN:
	case NFA_ZOPEN1:
	case NFA_ZOPEN2:
	case NFA_ZOPEN3:
	case NFA_ZOPEN4:
	case NFA_ZOPEN5:
	case NFA_ZOPEN6:
	case NFA_ZOPEN7:
	case NFA_ZOPEN8:
	case NFA_ZOPEN9:
#endif
	case NFA_ZSTART:
	    if (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    else if (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    else
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = NULL;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    if (REG_MULTI)
	    {
		if (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		else
		{
		    save_in_use = sub->in_use;
		    for (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		if (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		else
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					  (colnr_T)(rex.input - rex.line + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    else
	    {
		if (subidx < sub->in_use)
		{
		    save_ptr = sub->list.line[subidx].start;
		    save_in_use = -1;
		}
		else
		{
		    save_in_use = sub->in_use;
		    for (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.line[i].start = NULL;
			sub->list.line[i].end = NULL;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.line[subidx].start = rex.input + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg);
	    if (subs == NULL)
		break;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    if (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    else
#endif
		sub = &subs->norm;

	    if (save_in_use == -1)
	    {
		if (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		else
		    sub->list.line[subidx].start = save_ptr;
	    }
	    else
		sub->in_use = save_in_use;
	    break;

	case NFA_MCLOSE:
	    if (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.line[0].end != NULL))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg);
		break;
	    }
	    // FALLTHROUGH
	case NFA_MCLOSE1:
	case NFA_MCLOSE2:
	case NFA_MCLOSE3:
	case NFA_MCLOSE4:
	case NFA_MCLOSE5:
	case NFA_MCLOSE6:
	case NFA_MCLOSE7:
	case NFA_MCLOSE8:
	case NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	case NFA_ZCLOSE:
	case NFA_ZCLOSE1:
	case NFA_ZCLOSE2:
	case NFA_ZCLOSE3:
	case NFA_ZCLOSE4:
	case NFA_ZCLOSE5:
	case NFA_ZCLOSE6:
	case NFA_ZCLOSE7:
	case NFA_ZCLOSE8:
	case NFA_ZCLOSE9:
#endif
	case NFA_ZEND:
	    if (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    else if (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    else
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    if (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    if (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		if (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		else
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.input - rex.line + off);
		}
		// avoid compiler warnings
		save_ptr = NULL;
	    }
	    else
	    {
		save_ptr = sub->list.line[subidx].end;
		sub->list.line[subidx].end = rex.input + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg);
	    if (subs == NULL)
		break;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    if (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    else
#endif
		sub = &subs->norm;

	    if (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    else
		sub->list.line[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    break;
    }
    --depth;
    return subs;
}