nfa_max_width(nfa_state_T *startstate, int depth)
{
    int		    l, r;
    nfa_state_T	    *state = startstate;
    int		    len = 0;

    // detect looping in a NFA_SPLIT
    if (depth > 4)
	return -1;

    while (state != NULL)
    {
	switch (state->c)
	{
	    case NFA_END_INVISIBLE:
	    case NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		return len;

	    case NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		if (l < 0 || r < 0)
		    return -1;
		return len + (l > r ? l : r);

	    case NFA_ANY:
	    case NFA_START_COLL:
	    case NFA_START_NEG_COLL:
		// matches some character, including composing chars
		if (enc_utf8)
		    len += MB_MAXBYTES;
		else if (has_mbyte)
		    len += 2;
		else
		    ++len;
		if (state->c != NFA_ANY)
		{
		    // skip over the characters
		    state = state->out1->out;
		    continue;
		}
		break;

	    case NFA_DIGIT:
	    case NFA_WHITE:
	    case NFA_HEX:
	    case NFA_OCTAL:
		// ascii
		++len;
		break;

	    case NFA_IDENT:
	    case NFA_SIDENT:
	    case NFA_KWORD:
	    case NFA_SKWORD:
	    case NFA_FNAME:
	    case NFA_SFNAME:
	    case NFA_PRINT:
	    case NFA_SPRINT:
	    case NFA_NWHITE:
	    case NFA_NDIGIT:
	    case NFA_NHEX:
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
	    case NFA_ANY_COMPOSING:
		// possibly non-ascii
		if (has_mbyte)
		    len += 3;
		else
		    ++len;
		break;

	    case NFA_START_INVISIBLE:
	    case NFA_START_INVISIBLE_NEG:
	    case NFA_START_INVISIBLE_BEFORE:
	    case NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		continue;

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
	    case NFA_NEWL:
	    case NFA_SKIP:
		// unknown width
		return -1;

	    case NFA_BOL:
	    case NFA_EOL:
	    case NFA_BOF:
	    case NFA_EOF:
	    case NFA_BOW:
	    case NFA_EOW:
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
	    case NFA_NOPEN:
	    case NFA_NCLOSE:

	    case NFA_LNUM_GT:
	    case NFA_LNUM_LT:
	    case NFA_COL_GT:
	    case NFA_COL_LT:
	    case NFA_VCOL_GT:
	    case NFA_VCOL_LT:
	    case NFA_MARK_GT:
	    case NFA_MARK_LT:
	    case NFA_VISUAL:
	    case NFA_LNUM:
	    case NFA_CURSOR:
	    case NFA_COL:
	    case NFA_VCOL:
	    case NFA_MARK:

	    case NFA_ZSTART:
	    case NFA_ZEND:
	    case NFA_OPT_CHARS:
	    case NFA_EMPTY:
	    case NFA_START_PATTERN:
	    case NFA_END_PATTERN:
	    case NFA_COMPOSING:
	    case NFA_END_COMPOSING:
		// zero-width
		break;

	    default:
		if (state->c < 0)
		    // don't know what this is
		    return -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		break;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    return -1;
}