lookup_fullname(const char *find)
{
    int state = -1;

    for (;;) {
	int count = 0;
	NCURSES_CONST char *const *names;

	switch (++state) {
	case BOOLEAN:
	    names = boolfnames;
	    break;
	case STRING:
	    names = strfnames;
	    break;
	case NUMBER:
	    names = numfnames;
	    break;
	default:
	    return NOTFOUND;
	}

	for (count = 0; names[count] != 0; count++) {
	    if (!strcmp(names[count], find)) {
		struct name_table_entry const *entry_ptr = _nc_get_table(FALSE);
		while (entry_ptr->nte_type != state
		       || entry_ptr->nte_index != count)
		    entry_ptr++;
		return entry_ptr;
	    }
	}
    }
}