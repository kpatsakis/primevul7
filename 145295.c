trunc_left(
	const char *	src,
	size_t		width
	)
{
	size_t	sl;
	char *	out;


	sl = strlen(src);
	if (sl > width && LIB_BUFLENGTH - 1 > width && width > 1) {
		LIB_GETBUF(out);
		out[0] = '_';
		memcpy(&out[1], &src[sl + 1 - width], width);

		return out;
	}

	return src;
}