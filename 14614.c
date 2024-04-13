argv_next(int *argcp, char ***argvp)
{
	char *ret = (*argvp)[0];

	if (*argcp > 0 && ret != NULL) {
		(*argcp)--;
		(*argvp)++;
	}
	return ret;
}