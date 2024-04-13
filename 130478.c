purged_acs(TERMTYPE2 *tterm)
{
    bool result = FALSE;

    if (VALID_STRING(acs_chars)) {
	if (!one_one_mapping(acs_chars)) {
	    enter_alt_charset_mode = ABSENT_STRING;
	    exit_alt_charset_mode = ABSENT_STRING;
	    SHOW_WHY("# (rmacs/smacs removed for consistency)\n");
	}
	result = TRUE;
    }
    return result;
}