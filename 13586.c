nfa_set_code(int c)
{
    int	    addnl = FALSE;

    if (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    switch (c)
    {
	case NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); break;
	case NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); break;
	case NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); break;
	case NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); break;
	case NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); break;
	case NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); break;

	case NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); break;
	case NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); break;
	case NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); break;
	case NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); break;
	case NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); break;
	case NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); break;
	case NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); break;
	case NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); break;
	case NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); break;
#ifdef FEAT_SYN_HL
	case NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); break;
	case NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); break;
	case NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); break;
	case NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); break;
	case NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); break;
	case NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); break;
	case NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); break;
	case NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); break;
	case NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); break;
#endif
	case NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); break;

	case NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); break;
	case NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); break;
	case NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); break;
	case NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); break;
	case NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); break;

	case NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); break;
	case NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); break;
	case NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); break;
	case NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); break;
	case NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); break;
	case NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); break;
	case NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); break;
	case NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); break;
	case NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); break;
	case NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); break;
	case NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); break;
	case NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); break;
	case NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); break;
	case NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); break;

	case NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); break;
	case NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); break;
	case NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); break;

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
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    break;
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
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    break;
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
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    break;
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
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    break;
#endif
	case NFA_EOL:		STRCPY(code, "NFA_EOL "); break;
	case NFA_BOL:		STRCPY(code, "NFA_BOL "); break;
	case NFA_EOW:		STRCPY(code, "NFA_EOW "); break;
	case NFA_BOW:		STRCPY(code, "NFA_BOW "); break;
	case NFA_EOF:		STRCPY(code, "NFA_EOF "); break;
	case NFA_BOF:		STRCPY(code, "NFA_BOF "); break;
	case NFA_LNUM:		STRCPY(code, "NFA_LNUM "); break;
	case NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); break;
	case NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); break;
	case NFA_COL:		STRCPY(code, "NFA_COL "); break;
	case NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); break;
	case NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); break;
	case NFA_VCOL:		STRCPY(code, "NFA_VCOL "); break;
	case NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); break;
	case NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); break;
	case NFA_MARK:		STRCPY(code, "NFA_MARK "); break;
	case NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); break;
	case NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); break;
	case NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); break;
	case NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); break;
	case NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); break;

	case NFA_STAR:		STRCPY(code, "NFA_STAR "); break;
	case NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); break;
	case NFA_QUEST:		STRCPY(code, "NFA_QUEST"); break;
	case NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); break;
	case NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); break;
	case NFA_OR:		STRCPY(code, "NFA_OR"); break;

	case NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); break;
	case NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); break;
	case NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); break;
	case NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); break;
	case NFA_RANGE:		STRCPY(code, "NFA_RANGE"); break;
	case NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); break;
	case NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); break;

	case NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); break;
	case NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); break;
	case NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); break;
	case NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); break;
	case NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); break;
	case NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); break;
	case NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); break;
	case NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); break;
	case NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); break;
	case NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); break;
	case NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); break;
	case NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); break;
	case NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); break;
	case NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); break;
	case NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); break;
	case NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); break;
	case NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); break;
	case NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); break;
	case NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); break;

	case NFA_ANY:	STRCPY(code, "NFA_ANY"); break;
	case NFA_IDENT:	STRCPY(code, "NFA_IDENT"); break;
	case NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); break;
	case NFA_KWORD:	STRCPY(code, "NFA_KWORD"); break;
	case NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); break;
	case NFA_FNAME:	STRCPY(code, "NFA_FNAME"); break;
	case NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); break;
	case NFA_PRINT:	STRCPY(code, "NFA_PRINT"); break;
	case NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); break;
	case NFA_WHITE:	STRCPY(code, "NFA_WHITE"); break;
	case NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); break;
	case NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); break;
	case NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); break;
	case NFA_HEX:	STRCPY(code, "NFA_HEX"); break;
	case NFA_NHEX:	STRCPY(code, "NFA_NHEX"); break;
	case NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); break;
	case NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); break;
	case NFA_WORD:	STRCPY(code, "NFA_WORD"); break;
	case NFA_NWORD:	STRCPY(code, "NFA_NWORD"); break;
	case NFA_HEAD:	STRCPY(code, "NFA_HEAD"); break;
	case NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); break;
	case NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); break;
	case NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); break;
	case NFA_LOWER:	STRCPY(code, "NFA_LOWER"); break;
	case NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); break;
	case NFA_UPPER:	STRCPY(code, "NFA_UPPER"); break;
	case NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); break;
	case NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); break;
	case NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); break;
	case NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); break;
	case NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); break;

	default:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    if (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}