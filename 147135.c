guessContentTypeFromTable(struct table2 *table, char *filename)
{
    struct table2 *t;
    char *p;
    if (table == NULL)
	return NULL;
    p = &filename[strlen(filename) - 1];
    while (filename < p && *p != '.')
	p--;
    if (p == filename)
	return NULL;
    p++;
    for (t = table; t->item1; t++) {
	if (!strcmp(p, t->item1))
	    return t->item2;
    }
    for (t = table; t->item1; t++) {
	if (!strcasecmp(p, t->item1))
	    return t->item2;
    }
    return NULL;
}