static const char *binary_strcspn(const char *s, const char *e, const char *chars) {
	while (*chars) {
		const char *p = memchr(s, *chars, e - s);
		if (p) {
			e = p;
		}
		chars++;
	}
	return e;
}