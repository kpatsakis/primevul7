argv_free(char **av, int ac)
{
	int i;

	if (av == NULL)
		return;
	for (i = 0; i < ac; i++)
		free(av[i]);
	free(av);
}