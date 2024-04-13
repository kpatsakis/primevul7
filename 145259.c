trunc_right(
	const char *	src,
	size_t		width
	)
{
	size_t	sl;
	char *	out;


	sl = strlen(src);
	if (sl > width && LIB_BUFLENGTH - 1 > width && width > 0) {
		LIB_GETBUF(out);
		memcpy(out, src, width);
		out[width] = '\0';

		return out;
	}

	return src;
}