_nc_capcmp(const char *s, const char *t)
/* compare two string capabilities, stripping out padding */
{
    bool ok_s = VALID_STRING(s);
    bool ok_t = VALID_STRING(t);

    if (ok_s && ok_t) {
	for (;;) {
	    if (s[0] == '$' && s[1] == '<') {
		for (s += 2;; s++) {
		    if (!(isdigit(UChar(*s))
			  || *s == '.'
			  || *s == '*'
			  || *s == '/'
			  || *s == '>')) {
			break;
		    }
		}
	    }

	    if (t[0] == '$' && t[1] == '<') {
		for (t += 2;; t++) {
		    if (!(isdigit(UChar(*t))
			  || *t == '.'
			  || *t == '*'
			  || *t == '/'
			  || *t == '>')) {
			break;
		    }
		}
	    }

	    /* we've now pushed s and t past any padding they pointed at */

	    if (*s == '\0' && *t == '\0')
		return (0);

	    if (*s != *t)
		return (*t - *s);

	    /* else *s == *t but one is not NUL, so continue */
	    s++, t++;
	}
    } else if (ok_s || ok_t) {
	return 1;
    }
    return 0;
}