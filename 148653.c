contains(const char *list, const char *s) {
	int n = strlen(s);

	while (list && *list) {
		if (strncmp(list, s, n) == 0 &&
		    (list[n] == 0 || list[n] == ','))
			return 1;
		while (*list && *list++ != ',') ;
	}
	return 0;
}