static void handle_refs(struct rev_info *revs, unsigned flags,
		int (*for_each)(each_ref_fn, void *))
{
	struct all_refs_cb cb;
	cb.all_revs = revs;
	cb.all_flags = flags;
	for_each(handle_one_ref, &cb);
}