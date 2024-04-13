static void prepare_show_merge(struct rev_info *revs)
{
	struct commit_list *bases;
	struct commit *head, *other;
	unsigned char sha1[20];
	const char **prune = NULL;
	int i, prune_num = 1; /* counting terminating NULL */

	if (get_sha1("HEAD", sha1) || !(head = lookup_commit(sha1)))
		die("--merge without HEAD?");
	if (get_sha1("MERGE_HEAD", sha1) || !(other = lookup_commit(sha1)))
		die("--merge without MERGE_HEAD?");
	add_pending_object(revs, &head->object, "HEAD");
	add_pending_object(revs, &other->object, "MERGE_HEAD");
	bases = get_merge_bases(head, other, 1);
	add_pending_commit_list(revs, bases, UNINTERESTING);
	free_commit_list(bases);
	head->object.flags |= SYMMETRIC_LEFT;

	if (!active_nr)
		read_cache();
	for (i = 0; i < active_nr; i++) {
		struct cache_entry *ce = active_cache[i];
		if (!ce_stage(ce))
			continue;
		if (ce_path_match(ce, revs->prune_data)) {
			prune_num++;
			prune = xrealloc(prune, sizeof(*prune) * prune_num);
			prune[prune_num-2] = ce->name;
			prune[prune_num-1] = NULL;
		}
		while ((i+1 < active_nr) &&
		       ce_same_name(ce, active_cache[i+1]))
			i++;
	}
	revs->prune_data = prune;
	revs->limited = 1;
}