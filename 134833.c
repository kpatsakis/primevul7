STATIC bool
S_new_regcurly(const char *s, const char *e)
{
    /* This is a temporary function designed to match the most lenient form of
     * a {m,n} quantifier we ever envision, with either number omitted, and
     * spaces anywhere between/before/after them.
     *
     * If this function fails, then the string it matches is very unlikely to
     * ever be considered a valid quantifier, so we can allow the '{' that
     * begins it to be considered as a literal */

    bool has_min = FALSE;
    bool has_max = FALSE;

    PERL_ARGS_ASSERT_NEW_REGCURLY;

    if (s >= e || *s++ != '{')
	return FALSE;

    while (s < e && isSPACE(*s)) {
        s++;
    }
    while (s < e && isDIGIT(*s)) {
        has_min = TRUE;
        s++;
    }
    while (s < e && isSPACE(*s)) {
        s++;
    }

    if (*s == ',') {
	s++;
        while (s < e && isSPACE(*s)) {
            s++;
        }
        while (s < e && isDIGIT(*s)) {
            has_max = TRUE;
            s++;
        }
        while (s < e && isSPACE(*s)) {
            s++;
        }
    }

    return s < e && *s == '}' && (has_min || has_max);