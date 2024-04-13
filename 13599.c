nfa_postprocess(nfa_regprog_T *prog)
{
    int i;
    int c;

    for (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	if (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    int directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    if (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    else
	    {
		int ch_invisible = failure_chance(prog->state[i].out, 0);
		int ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		if (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    if (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    else
			directly = ch_follows * 10 < ch_invisible;
		}
		else
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    if (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}