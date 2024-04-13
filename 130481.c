_nc_extend_names(ENTRY * entryp, const char *name, int token_type)
{
    static struct name_table_entry temp;
    TERMTYPE2 *tp = &(entryp->tterm);
    unsigned offset = 0;
    unsigned actual;
    unsigned tindex;
    unsigned first, last, n;
    bool found;

    switch (token_type) {
    case BOOLEAN:
	first = 0;
	last = tp->ext_Booleans;
	offset = tp->ext_Booleans;
	tindex = tp->num_Booleans;
	break;
    case NUMBER:
	first = tp->ext_Booleans;
	last = tp->ext_Numbers + first;
	offset = (unsigned) (tp->ext_Booleans + tp->ext_Numbers);
	tindex = tp->num_Numbers;
	break;
    case STRING:
	first = (unsigned) (tp->ext_Booleans + tp->ext_Numbers);
	last = tp->ext_Strings + first;
	offset = (unsigned) (tp->ext_Booleans + tp->ext_Numbers + tp->ext_Strings);
	tindex = tp->num_Strings;
	break;
    case CANCEL:
	actual = NUM_EXT_NAMES(tp);
	for (n = 0; n < actual; n++) {
	    if (!strcmp(name, tp->ext_Names[n])) {
		if (n > (unsigned) (tp->ext_Booleans + tp->ext_Numbers)) {
		    token_type = STRING;
		} else if (n > tp->ext_Booleans) {
		    token_type = NUMBER;
		} else {
		    token_type = BOOLEAN;
		}
		return _nc_extend_names(entryp, name, token_type);
	    }
	}
	/* Well, we are given a cancel for a name that we don't recognize */
	return _nc_extend_names(entryp, name, STRING);
    default:
	return 0;
    }

    /* Adjust the 'offset' (insertion-point) to keep the lists of extended
     * names sorted.
     */
    for (n = first, found = FALSE; n < last; n++) {
	int cmp = strcmp(tp->ext_Names[n], name);
	if (cmp == 0)
	    found = TRUE;
	if (cmp >= 0) {
	    offset = n;
	    tindex = n - first;
	    switch (token_type) {
	    case BOOLEAN:
		tindex += BOOLCOUNT;
		break;
	    case NUMBER:
		tindex += NUMCOUNT;
		break;
	    case STRING:
		tindex += STRCOUNT;
		break;
	    }
	    break;
	}
    }

#define for_each_value(max) \
	for (last = (unsigned) (max - 1); last > tindex; last--)

    if (!found) {
	switch (token_type) {
	case BOOLEAN:
	    tp->ext_Booleans++;
	    tp->num_Booleans++;
	    TYPE_REALLOC(NCURSES_SBOOL, tp->num_Booleans, tp->Booleans);
	    for_each_value(tp->num_Booleans)
		tp->Booleans[last] = tp->Booleans[last - 1];
	    break;
	case NUMBER:
	    tp->ext_Numbers++;
	    tp->num_Numbers++;
	    TYPE_REALLOC(NCURSES_INT2, tp->num_Numbers, tp->Numbers);
	    for_each_value(tp->num_Numbers)
		tp->Numbers[last] = tp->Numbers[last - 1];
	    break;
	case STRING:
	    tp->ext_Strings++;
	    tp->num_Strings++;
	    TYPE_REALLOC(char *, tp->num_Strings, tp->Strings);
	    for_each_value(tp->num_Strings)
		tp->Strings[last] = tp->Strings[last - 1];
	    break;
	}
	actual = NUM_EXT_NAMES(tp);
	TYPE_REALLOC(char *, actual, tp->ext_Names);
	while (--actual > offset)
	    tp->ext_Names[actual] = tp->ext_Names[actual - 1];
	tp->ext_Names[offset] = _nc_save_str(name);
    }

    temp.nte_name = tp->ext_Names[offset];
    temp.nte_type = token_type;
    temp.nte_index = (short) tindex;
    temp.nte_link = -1;

    return &temp;
}