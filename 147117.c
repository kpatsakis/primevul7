loadURIMethods(char *filename)
{
    FILE *f;
    int i, n;
    Str tmp;
    struct table2 *um;
    char *up, *p;

    f = fopen(expandPath(filename), "r");
    if (f == NULL)
	return NULL;
    i = 0;
    while (tmp = Strfgets(f), tmp->length > 0) {
	if (tmp->ptr[0] != '#')
	    i++;
    }
    fseek(f, 0, 0);
    n = i;
    um = New_N(struct table2, n + 1);
    i = 0;
    while (tmp = Strfgets(f), tmp->length > 0) {
	if (tmp->ptr[0] == '#')
	    continue;
	while (IS_SPACE(Strlastchar(tmp)))
	    Strshrink(tmp, 1);
	for (up = p = tmp->ptr; *p != '\0'; p++) {
	    if (*p == ':') {
		um[i].item1 = Strnew_charp_n(up, p - up)->ptr;
		p++;
		break;
	    }
	}
	if (*p == '\0')
	    continue;
	while (*p != '\0' && IS_SPACE(*p))
	    p++;
	um[i].item2 = Strnew_charp(p)->ptr;
	i++;
    }
    um[i].item1 = NULL;
    um[i].item2 = NULL;
    fclose(f);
    return um;
}