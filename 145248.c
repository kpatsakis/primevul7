atoascii(
	const char *in,
	size_t in_octets,
	char *out,
	size_t out_octets
	)
{
	const u_char *	pchIn;
	const u_char *	pchInLimit;
	u_char *	pchOut;
	u_char		c;

	pchIn = (const u_char *)in;
	pchInLimit = pchIn + in_octets;
	pchOut = (u_char *)out;

	if (NULL == pchIn) {
		if (0 < out_octets)
			*pchOut = '\0';
		return;
	}

#define	ONEOUT(c)					\
do {							\
	if (0 == --out_octets) {			\
		*pchOut = '\0';				\
		return;					\
	}						\
	*pchOut++ = (c);				\
} while (0)

	for (	; pchIn < pchInLimit; pchIn++) {
		c = *pchIn;
		if ('\0' == c)
			break;
		if (c & 0x80) {
			ONEOUT('M');
			ONEOUT('-');
			c &= 0x7f;
		}
		if (c < ' ') {
			ONEOUT('^');
			ONEOUT((u_char)(c + '@'));
		} else if (0x7f == c) {
			ONEOUT('^');
			ONEOUT('?');
		} else
			ONEOUT(c);
	}
	ONEOUT('\0');

#undef ONEOUT
}