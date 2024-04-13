static int binder_inc_ref_olocked(struct binder_ref *ref, int strong,
				  struct list_head *target_list)
{
	int ret;

	if (strong) {
		if (ref->data.strong == 0) {
			ret = binder_inc_node(ref->node, 1, 1, target_list);
			if (ret)
				return ret;
		}
		ref->data.strong++;
	} else {
		if (ref->data.weak == 0) {
			ret = binder_inc_node(ref->node, 0, 1, target_list);
			if (ret)
				return ret;
		}
		ref->data.weak++;
	}
	return 0;
}