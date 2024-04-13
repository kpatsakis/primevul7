struct commit *get_revision(struct rev_info *revs)
{
	struct commit *c = get_revision_internal(revs);
	if (c && revs->graph)
		graph_update(revs->graph, c);
	return c;
}