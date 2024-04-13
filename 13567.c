match_follows(nfa_state_T *startstate, int depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    if (depth > 10)
	return FALSE;

    while (state != NULL)
    {
	switch (state->c)
	{
	    case NFA_MATCH:
	    case NFA_MCLOSE:
	    case NFA_END_INVISIBLE:
	    case NFA_END_INVISIBLE_NEG:
	    case NFA_END_PATTERN:
		return TRUE;

	    case NFA_SPLIT:
		return match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    case NFA_START_INVISIBLE:
	    case NFA_START_INVISIBLE_FIRST:
	    case NFA_START_INVISIBLE_BEFORE:
	    case NFA_START_INVISIBLE_BEFORE_FIRST:
	    case NFA_START_INVISIBLE_NEG:
	    case NFA_START_INVISIBLE_NEG_FIRST:
	    case NFA_START_INVISIBLE_BEFORE_NEG:
	    case NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    case NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		continue;

	    case NFA_ANY:
	    case NFA_ANY_COMPOSING:
	    case NFA_IDENT:
	    case NFA_SIDENT:
	    case NFA_KWORD:
	    case NFA_SKWORD:
	    case NFA_FNAME:
	    case NFA_SFNAME:
	    case NFA_PRINT:
	    case NFA_SPRINT:
	    case NFA_WHITE:
	    case NFA_NWHITE:
	    case NFA_DIGIT:
	    case NFA_NDIGIT:
	    case NFA_HEX:
	    case NFA_NHEX:
	    case NFA_OCTAL:
	    case NFA_NOCTAL:
	    case NFA_WORD:
	    case NFA_NWORD:
	    case NFA_HEAD:
	    case NFA_NHEAD:
	    case NFA_ALPHA:
	    case NFA_NALPHA:
	    case NFA_LOWER:
	    case NFA_NLOWER:
	    case NFA_UPPER:
	    case NFA_NUPPER:
	    case NFA_LOWER_IC:
	    case NFA_NLOWER_IC:
	    case NFA_UPPER_IC:
	    case NFA_NUPPER_IC:
	    case NFA_START_COLL:
	    case NFA_START_NEG_COLL:
	    case NFA_NEWL:
		// state will advance input
		return FALSE;

	    default:
		if (state->c > 0)
		    // state will advance input
		    return FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		break;
	}
	state = state->out;
    }
    return FALSE;
}