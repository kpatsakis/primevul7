_binder_node_inner_unlock(struct binder_node *node, int line)
{
	struct binder_proc *proc = node->proc;

	binder_debug(BINDER_DEBUG_SPINLOCKS,
		     "%s: line=%d\n", __func__, line);
	if (proc)
		binder_inner_proc_unlock(proc);
	spin_unlock(&node->lock);
}