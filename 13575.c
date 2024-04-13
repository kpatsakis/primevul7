nfa_get_regstart(nfa_state_T *start, int depth)
{
    nfa_state_T *p = start;

    if (depth > 4)
	return 0;

    while (p != NULL)
    {
	switch (p->c)
	{
	    // all kinds of zero-width matches
	    case NFA_BOL:
	    case NFA_BOF:
	    case NFA_BOW:
	    case NFA_EOW:
	    case NFA_ZSTART:
	    case NFA_ZEND:
	    case NFA_CURSOR:
	    case NFA_VISUAL:
	    case NFA_LNUM:
	    case NFA_LNUM_GT:
	    case NFA_LNUM_LT:
	    case NFA_COL:
	    case NFA_COL_GT:
	    case NFA_COL_LT:
	    case NFA_VCOL:
	    case NFA_VCOL_GT:
	    case NFA_VCOL_LT:
	    case NFA_MARK:
	    case NFA_MARK_GT:
	    case NFA_MARK_LT:

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
	    case NFA_NOPEN:
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
		p = p->out;
		break;

	    case NFA_SPLIT:
	    {
		int c1 = nfa_get_regstart(p->out, depth + 1);
		int c2 = nfa_get_regstart(p->out1, depth + 1);

		if (c1 == c2)
		    return c1; // yes!
		return 0;
	    }

	    default:
		if (p->c > 0)
		    return p->c; // yes!
		return 0;
	}
    }
    return 0;
}