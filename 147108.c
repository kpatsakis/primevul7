initURIMethods()
{
    TextList *methodmap_list = NULL;
    TextListItem *tl;
    int i;

    if (non_null(urimethodmap_files))
	methodmap_list = make_domain_list(urimethodmap_files);
    if (methodmap_list == NULL)
	return;
    urimethods = New_N(struct table2 *, (methodmap_list->nitem + 1));
    for (i = 0, tl = methodmap_list->first; tl; tl = tl->next) {
	urimethods[i] = loadURIMethods(tl->ptr);
	if (urimethods[i])
	    i++;
    }
    urimethods[i] = NULL;
}