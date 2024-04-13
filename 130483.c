expected_type(const char *name, int token_type, bool silent)
{
    struct user_table_entry const *entry = _nc_find_user_entry(name);
    bool result = TRUE;
    if ((entry != 0) && (token_type != CANCEL)) {
	int have_type = (1 << token_type);
	if (!(entry->ute_type & have_type)) {
	    if (!silent)
		_nc_warning("expected %s-type for %s, have %s",
			    usertype2s(entry->ute_type),
			    name,
			    usertype2s(have_type));
	    result = FALSE;
	}
    }
    return result;
}