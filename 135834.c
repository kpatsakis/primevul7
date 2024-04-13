static int count_paths(const char **paths)
{
	int i = 0;
	while (*paths++)
		i++;
	return i;
}