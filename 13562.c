nfa_recognize_char_class(char_u *start, char_u *end, int extra_newl)
{
#   define CLASS_not		0x80
#   define CLASS_af		0x40
#   define CLASS_AF		0x20
#   define CLASS_az		0x10
#   define CLASS_AZ		0x08
#   define CLASS_o7		0x04
#   define CLASS_o9		0x02
#   define CLASS_underscore	0x01

    int		newl = FALSE;
    char_u	*p;
    int		config = 0;

    if (extra_newl == TRUE)
	newl = TRUE;

    if (*end != ']')
	return FAIL;
    p = start;
    if (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    while (p < end)
    {
	if (p + 2 < end && *(p + 1) == '-')
	{
	    switch (*p)
	    {
		case '0':
		    if (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			break;
		    }
		    if (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			break;
		    }
		    return FAIL;

		case 'a':
		    if (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			break;
		    }
		    if (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			break;
		    }
		    return FAIL;

		case 'A':
		    if (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			break;
		    }
		    if (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			break;
		    }
		    return FAIL;

		default:
		    return FAIL;
	    }
	    p += 3;
	}
	else if (p + 1 < end && *p == '\\' && *(p + 1) == 'n')
	{
	    newl = TRUE;
	    p += 2;
	}
	else if (*p == '_')
	{
	    config |= CLASS_underscore;
	    p ++;
	}
	else if (*p == '\n')
	{
	    newl = TRUE;
	    p ++;
	}
	else
	    return FAIL;
    } // while (p < end)

    if (p != end)
	return FAIL;

    if (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    switch (config)
    {
	case CLASS_o9:
	    return extra_newl + NFA_DIGIT;
	case CLASS_not |  CLASS_o9:
	    return extra_newl + NFA_NDIGIT;
	case CLASS_af | CLASS_AF | CLASS_o9:
	    return extra_newl + NFA_HEX;
	case CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    return extra_newl + NFA_NHEX;
	case CLASS_o7:
	    return extra_newl + NFA_OCTAL;
	case CLASS_not | CLASS_o7:
	    return extra_newl + NFA_NOCTAL;
	case CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    return extra_newl + NFA_WORD;
	case CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    return extra_newl + NFA_NWORD;
	case CLASS_az | CLASS_AZ | CLASS_underscore:
	    return extra_newl + NFA_HEAD;
	case CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    return extra_newl + NFA_NHEAD;
	case CLASS_az | CLASS_AZ:
	    return extra_newl + NFA_ALPHA;
	case CLASS_not | CLASS_az | CLASS_AZ:
	    return extra_newl + NFA_NALPHA;
	case CLASS_az:
	   return extra_newl + NFA_LOWER_IC;
	case CLASS_not | CLASS_az:
	    return extra_newl + NFA_NLOWER_IC;
	case CLASS_AZ:
	    return extra_newl + NFA_UPPER_IC;
	case CLASS_not | CLASS_AZ:
	    return extra_newl + NFA_NUPPER_IC;
    }
    return FAIL;
}