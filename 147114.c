add_index_file(ParsedURL *pu, URLFile *uf)
{
    char *p, *q;
    TextList *index_file_list = NULL;
    TextListItem *ti;

    if (non_null(index_file))
	index_file_list = make_domain_list(index_file);
    if (index_file_list == NULL) {
	uf->stream = NULL;
	return;
    }
    for (ti = index_file_list->first; ti; ti = ti->next) {
	p = Strnew_m_charp(pu->file, "/", file_quote(ti->ptr), NULL)->ptr;
	p = cleanupName(p);
	q = cleanupName(file_unquote(p));
	examineFile(q, uf);
	if (uf->stream != NULL) {
	    pu->file = p;
	    pu->real_file = q;
	    return;
	}
    }
}