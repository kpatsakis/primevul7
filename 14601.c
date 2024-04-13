opt_array_append2(const char *file, const int line, const char *directive,
    char ***array, int **iarray, u_int *lp, const char *s, int i)
{

	if (*lp >= INT_MAX)
		fatal("%s line %d: Too many %s entries", file, line, directive);

	if (iarray != NULL) {
		*iarray = xrecallocarray(*iarray, *lp, *lp + 1,
		    sizeof(**iarray));
		(*iarray)[*lp] = i;
	}

	*array = xrecallocarray(*array, *lp, *lp + 1, sizeof(**array));
	(*array)[*lp] = xstrdup(s);
	(*lp)++;
}