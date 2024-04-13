check_char_class(int class, int c)
{
    switch (class)
    {
	case NFA_CLASS_ALNUM:
	    if (c >= 1 && c < 128 && isalnum(c))
		return OK;
	    break;
	case NFA_CLASS_ALPHA:
	    if (c >= 1 && c < 128 && isalpha(c))
		return OK;
	    break;
	case NFA_CLASS_BLANK:
	    if (c == ' ' || c == '\t')
		return OK;
	    break;
	case NFA_CLASS_CNTRL:
	    if (c >= 1 && c <= 127 && iscntrl(c))
		return OK;
	    break;
	case NFA_CLASS_DIGIT:
	    if (VIM_ISDIGIT(c))
		return OK;
	    break;
	case NFA_CLASS_GRAPH:
	    if (c >= 1 && c <= 127 && isgraph(c))
		return OK;
	    break;
	case NFA_CLASS_LOWER:
	    if (MB_ISLOWER(c) && c != 170 && c != 186)
		return OK;
	    break;
	case NFA_CLASS_PRINT:
	    if (vim_isprintc(c))
		return OK;
	    break;
	case NFA_CLASS_PUNCT:
	    if (c >= 1 && c < 128 && ispunct(c))
		return OK;
	    break;
	case NFA_CLASS_SPACE:
	    if ((c >= 9 && c <= 13) || (c == ' '))
		return OK;
	    break;
	case NFA_CLASS_UPPER:
	    if (MB_ISUPPER(c))
		return OK;
	    break;
	case NFA_CLASS_XDIGIT:
	    if (vim_isxdigit(c))
		return OK;
	    break;
	case NFA_CLASS_TAB:
	    if (c == '\t')
		return OK;
	    break;
	case NFA_CLASS_RETURN:
	    if (c == '\r')
		return OK;
	    break;
	case NFA_CLASS_BACKSPACE:
	    if (c == '\b')
		return OK;
	    break;
	case NFA_CLASS_ESCAPE:
	    if (c == '\033')
		return OK;
	    break;
	case NFA_CLASS_IDENT:
	    if (vim_isIDc(c))
		return OK;
	    break;
	case NFA_CLASS_KEYWORD:
	    if (reg_iswordc(c))
		return OK;
	    break;
	case NFA_CLASS_FNAME:
	    if (vim_isfilec(c))
		return OK;
	    break;

	default:
	    // should not be here :P
	    siemsg(_(e_ill_char_class), class);
	    return FAIL;
    }
    return FAIL;
}