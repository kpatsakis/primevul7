has_params(const char *src)
{
    bool result = FALSE;
    int len = (int) strlen(src);
    int n;
    bool ifthen = FALSE;
    bool params = FALSE;

    for (n = 0; n < len - 1; ++n) {
	if (!strncmp(src + n, "%p", (size_t) 2)) {
	    params = TRUE;
	} else if (!strncmp(src + n, "%;", (size_t) 2)) {
	    ifthen = TRUE;
	    result = params;
	    break;
	}
    }
    if (!ifthen) {
	result = ((len > 50) && params);
    }
    return result;
}