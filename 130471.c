fill_spaces(const char *src)
{
    const char *fill = "\\s";
    size_t need = strlen(src);
    size_t size = strlen(fill);
    char *result = 0;
    int pass;
    int s, d;
    for (pass = 0; pass < 2; ++pass) {
	for (s = d = 0; src[s] != '\0'; ++s) {
	    if (src[s] == ' ') {
		if (pass) {
		    _nc_STRCPY(&result[d], fill, need + 1 - d);
		    d += (int) size;
		} else {
		    need += size;
		}
	    } else {
		if (pass) {
		    result[d++] = src[s];
		} else {
		    ++d;
		}
	    }
	}
	if (pass) {
	    result[d] = '\0';
	} else {
	    result = malloc(need + 1);
	    if (result == 0)
		failed("fill_spaces");
	}
    }
    return result;
}