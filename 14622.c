rtrim(char *s)
{
	size_t i;

	if ((i = strlen(s)) == 0)
		return;
	for (i--; i > 0; i--) {
		if (isspace((int)s[i]))
			s[i] = '\0';
	}
}