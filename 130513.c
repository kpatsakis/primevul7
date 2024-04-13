version_filter(PredType type, PredIdx idx)
/* filter out capabilities we may want to suppress */
{
    switch (tversion) {
    case V_ALLCAPS:		/* SVr4, XSI Curses */
	return (TRUE);

    case V_SVR1:		/* System V Release 1, Ultrix */
	switch (type) {
	case BOOLEAN:
	    return ((idx <= BOOL_IDX(xon_xoff)) ? TRUE : FALSE);
	case NUMBER:
	    return ((idx <= NUM_IDX(width_status_line)) ? TRUE : FALSE);
	case STRING:
	    return ((idx <= STR_IDX(prtr_non)) ? TRUE : FALSE);
	}
	break;

    case V_HPUX:		/* Hewlett-Packard */
	switch (type) {
	case BOOLEAN:
	    return ((idx <= BOOL_IDX(xon_xoff)) ? TRUE : FALSE);
	case NUMBER:
	    return ((idx <= NUM_IDX(label_width)) ? TRUE : FALSE);
	case STRING:
	    if (idx <= STR_IDX(prtr_non))
		return (TRUE);
	    else if (FNKEY(idx))	/* function keys */
		return (TRUE);
	    else if (idx == STR_IDX(plab_norm)
		     || idx == STR_IDX(label_on)
		     || idx == STR_IDX(label_off))
		return (TRUE);
	    else
		return (FALSE);
	}
	break;

    case V_AIX:		/* AIX */
	switch (type) {
	case BOOLEAN:
	    return ((idx <= BOOL_IDX(xon_xoff)) ? TRUE : FALSE);
	case NUMBER:
	    return ((idx <= NUM_IDX(width_status_line)) ? TRUE : FALSE);
	case STRING:
	    if (idx <= STR_IDX(prtr_non))
		return (TRUE);
	    else if (FNKEY(idx))	/* function keys */
		return (TRUE);
	    else
		return (FALSE);
	}
	break;

#define is_termcap(type) (OkIndex(idx, type##_from_termcap) && \
			  type##_from_termcap[idx])

    case V_BSD:		/* BSD */
	switch (type) {
	case BOOLEAN:
	    return is_termcap(bool);
	case NUMBER:
	    return is_termcap(num);
	case STRING:
	    return is_termcap(str);
	}
	break;
    }

    return (FALSE);		/* pacify the compiler */
}