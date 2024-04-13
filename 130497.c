valid_entryname(const char *name)
{
    bool result = TRUE;
    int ch;
    while ((ch = UChar(*name++)) != '\0') {
	if (ch <= ' ' || ch > '~' || ch == '/') {
	    result = FALSE;
	    break;
	}
    }
    return result;
}