nametrans(const char *name)
/* translate a capability name to termcap from terminfo */
{
    const struct name_table_entry *np;

    if ((np = _nc_find_entry(name, _nc_get_hash_table(0))) != 0) {
	switch (np->nte_type) {
	case BOOLEAN:
	    NameTrans(bool_from_termcap, boolcodes);
	    break;

	case NUMBER:
	    NameTrans(num_from_termcap, numcodes);
	    break;

	case STRING:
	    NameTrans(str_from_termcap, strcodes);
	    break;
	}
    }

    return (0);
}