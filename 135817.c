static int remove_space(char *line, int len)
{
	int i;
	char *dst = line;
	unsigned char c;

	for (i = 0; i < len; i++)
		if (!isspace((c = line[i])))
			*dst++ = c;

	return dst - line;
}