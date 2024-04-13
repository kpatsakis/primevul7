kill_string(TERMTYPE2 *tterm, char *cap)
{
    unsigned n;
    for (n = 0; n < NUM_STRINGS(tterm); ++n) {
	if (cap == tterm->Strings[n]) {
	    tterm->Strings[n] = ABSENT_STRING;
	    return TRUE;
	}
    }
    return FALSE;
}