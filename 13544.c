failure_chance(nfa_state_T *state, int depth)
{
    int c = state->c;
    int l, r;

    // detect looping
    if (depth > 4)
	return 1;

    switch (c)
    {
	case NFA_SPLIT:
	    if (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		return 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    return l < r ? l : r;

	case NFA_ANY:
	    // matches anything, unlikely to fail
	    return 1;

	case NFA_MATCH:
	case NFA_MCLOSE:
	case NFA_ANY_COMPOSING:
	    // empty match works always
	    return 0;

	case NFA_START_INVISIBLE:
	case NFA_START_INVISIBLE_FIRST:
	case NFA_START_INVISIBLE_NEG:
	case NFA_START_INVISIBLE_NEG_FIRST:
	case NFA_START_INVISIBLE_BEFORE:
	case NFA_START_INVISIBLE_BEFORE_FIRST:
	case NFA_START_INVISIBLE_BEFORE_NEG:
	case NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	case NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    return 5;

	case NFA_BOL:
	case NFA_EOL:
	case NFA_BOF:
	case NFA_EOF:
	case NFA_NEWL:
	    return 99;

	case NFA_BOW:
	case NFA_EOW:
	    return 90;

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
	case NFA_NOPEN:
	case NFA_MCLOSE1:
	case NFA_MCLOSE2:
	case NFA_MCLOSE3:
	case NFA_MCLOSE4:
	case NFA_MCLOSE5:
	case NFA_MCLOSE6:
	case NFA_MCLOSE7:
	case NFA_MCLOSE8:
	case NFA_MCLOSE9:
	case NFA_NCLOSE:
	    return failure_chance(state->out, depth + 1);

	case NFA_BACKREF1:
	case NFA_BACKREF2:
	case NFA_BACKREF3:
	case NFA_BACKREF4:
	case NFA_BACKREF5:
	case NFA_BACKREF6:
	case NFA_BACKREF7:
	case NFA_BACKREF8:
	case NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	case NFA_ZREF1:
	case NFA_ZREF2:
	case NFA_ZREF3:
	case NFA_ZREF4:
	case NFA_ZREF5:
	case NFA_ZREF6:
	case NFA_ZREF7:
	case NFA_ZREF8:
	case NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    return 94;

	case NFA_LNUM_GT:
	case NFA_LNUM_LT:
	case NFA_COL_GT:
	case NFA_COL_LT:
	case NFA_VCOL_GT:
	case NFA_VCOL_LT:
	case NFA_MARK_GT:
	case NFA_MARK_LT:
	case NFA_VISUAL:
	    // before/after positions don't match very often
	    return 85;

	case NFA_LNUM:
	    return 90;

	case NFA_CURSOR:
	case NFA_COL:
	case NFA_VCOL:
	case NFA_MARK:
	    // specific positions rarely match
	    return 98;

	case NFA_COMPOSING:
	    return 95;

	default:
	    if (c > 0)
		// character match fails often
		return 95;
    }

    // something else, includes character classes
    return 50;
}