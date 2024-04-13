make_domain_list(char *domain_list)
{
    char *p;
    Str tmp;
    TextList *domains = NULL;

    p = domain_list;
    tmp = Strnew_size(64);
    while (*p) {
	while (*p && IS_SPACE(*p))
	    p++;
	Strclear(tmp);
	while (*p && !IS_SPACE(*p) && *p != ',')
	    Strcat_char(tmp, *p++);
	if (tmp->length > 0) {
	    if (domains == NULL)
		domains = newTextList();
	    pushText(domains, tmp->ptr);
	}
	while (*p && IS_SPACE(*p))
	    p++;
	if (*p == ',')
	    p++;
    }
    return domains;
}