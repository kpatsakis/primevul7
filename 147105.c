initMimeTypes()
{
    int i;
    TextListItem *tl;

    if (non_null(mimetypes_files))
	mimetypes_list = make_domain_list(mimetypes_files);
    else
	mimetypes_list = NULL;
    if (mimetypes_list == NULL)
	return;
    UserMimeTypes = New_N(struct table2 *, mimetypes_list->nitem);
    for (i = 0, tl = mimetypes_list->first; tl; i++, tl = tl->next)
	UserMimeTypes[i] = loadMimeTypes(tl->ptr);
}