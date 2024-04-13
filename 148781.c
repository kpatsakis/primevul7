static int binder_ioctl_get_node_debug_info(struct binder_proc *proc,
				struct binder_node_debug_info *info)
{
	struct rb_node *n;
	binder_uintptr_t ptr = info->ptr;

	memset(info, 0, sizeof(*info));

	binder_inner_proc_lock(proc);
	for (n = rb_first(&proc->nodes); n != NULL; n = rb_next(n)) {
		struct binder_node *node = rb_entry(n, struct binder_node,
						    rb_node);
		if (node->ptr > ptr) {
			info->ptr = node->ptr;
			info->cookie = node->cookie;
			info->has_strong_ref = node->has_strong_ref;
			info->has_weak_ref = node->has_weak_ref;
			break;
		}
	}
	binder_inner_proc_unlock(proc);

	return 0;
}