static int handle_one_ref(const char *path, const unsigned char *sha1, int flag, void *cb_data)
{
	struct all_refs_cb *cb = cb_data;
	struct object *object = get_reference(cb->all_revs, path, sha1,
					      cb->all_flags);
	add_pending_object(cb->all_revs, object, path);
	return 0;
}