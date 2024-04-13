int check_name(char *name, int size)
{
	char *start = name;

	if(name[0] == '.') {
		if(name[1] == '.')
			name++;
		if(name[1] == '/' || name[1] == '\0')
			return FALSE;
	}

	while(name[0] != '/' && name[0] != '\0')
		name ++;

	if(name[0] == '/')
		return FALSE;

	if((name - start) != size)
		return FALSE;

	return TRUE;
}