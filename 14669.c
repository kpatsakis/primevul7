strcmp_maybe_null(const char *a, const char *b)
{
	if ((a == NULL && b != NULL) || (a != NULL && b == NULL))
		return 0;
	if (a != NULL && strcmp(a, b) != 0)
		return 0;
	return 1;
}