query_info_clear(struct query_info* m)
{
	free(m->qname);
	m->qname = NULL;
}