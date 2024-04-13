lowercase(char *s)
{
	for (; *s; s++)
		*s = tolower((u_char)*s);
}