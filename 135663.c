static void handle_one_reflog_commit(unsigned char *sha1, void *cb_data)
{
	struct all_refs_cb *cb = cb_data;
	if (!is_null_sha1(sha1)) {
		struct object *o = parse_object(sha1);
		if (o) {
			o->flags |= cb->all_flags;
			add_pending_object(cb->all_revs, o, "");
		}
		else if (!cb->warned_bad_reflog) {
			warning("reflog of '%s' references pruned commits",
				cb->name_for_errormsg);
			cb->warned_bad_reflog = 1;
		}
	}
}