loadMimeTypes(char *filename)
{
    FILE *f;
    char *d, *type;
    int i, n;
    Str tmp;
    struct table2 *mtypes;

    f = fopen(expandPath(filename), "r");
    if (f == NULL)
	return NULL;
    n = 0;
    while (tmp = Strfgets(f), tmp->length > 0) {
	d = tmp->ptr;
	if (d[0] != '#') {
	    d = strtok(d, " \t\n\r");
	    if (d != NULL) {
		d = strtok(NULL, " \t\n\r");
		for (i = 0; d != NULL; i++)
		    d = strtok(NULL, " \t\n\r");
		n += i;
	    }
	}
    }
    fseek(f, 0, 0);
    mtypes = New_N(struct table2, n + 1);
    i = 0;
    while (tmp = Strfgets(f), tmp->length > 0) {
	d = tmp->ptr;
	if (d[0] == '#')
	    continue;
	type = strtok(d, " \t\n\r");
	if (type == NULL)
	    continue;
	while (1) {
	    d = strtok(NULL, " \t\n\r");
	    if (d == NULL)
		break;
	    mtypes[i].item1 = Strnew_charp(d)->ptr;
	    mtypes[i].item2 = Strnew_charp(type)->ptr;
	    i++;
	}
    }
    mtypes[i].item1 = NULL;
    mtypes[i].item2 = NULL;
    fclose(f);
    return mtypes;
}