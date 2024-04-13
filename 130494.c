find_string(TERMTYPE2 *tterm, char *name)
{
    PredIdx n;
    for (n = 0; n < NUM_STRINGS(tterm); ++n) {
	if (version_filter(STRING, n)
	    && !strcmp(name, strnames[n])) {
	    char *cap = tterm->Strings[n];
	    if (VALID_STRING(cap)) {
		return cap;
	    }
	    break;
	}
    }
    return ABSENT_STRING;
}