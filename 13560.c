nfa_get_reganch(nfa_state_T *start, int depth)
{
    nfa_state_T *p = start;

    if (depth > 4)
	return 0;

    while (p != NULL)
    {
	switch (p->c)
	{
	    case NFA_BOL:
	    case NFA_BOF:
		return 1; // yes!

	    case NFA_ZSTART:
	    case NFA_ZEND:
	    case NFA_CURSOR:
	    case NFA_VISUAL:

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
		return nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    default:
		return 0; // noooo
	}
    }
    return 0;
}