static int binder_translate_handle(struct flat_binder_object *fp,
				   struct binder_transaction *t,
				   struct binder_thread *thread)
{
	struct binder_proc *proc = thread->proc;
	struct binder_proc *target_proc = t->to_proc;
	struct binder_node *node;
	struct binder_ref_data src_rdata;
	int ret = 0;

	node = binder_get_node_from_ref(proc, fp->handle,
			fp->hdr.type == BINDER_TYPE_HANDLE, &src_rdata);
	if (!node) {
		binder_user_error("%d:%d got transaction with invalid handle, %d\n",
				  proc->pid, thread->pid, fp->handle);
		return -EINVAL;
	}
	if (security_binder_transfer_binder(proc->tsk, target_proc->tsk)) {
		ret = -EPERM;
		goto done;
	}

	binder_node_lock(node);
	if (node->proc == target_proc) {
		if (fp->hdr.type == BINDER_TYPE_HANDLE)
			fp->hdr.type = BINDER_TYPE_BINDER;
		else
			fp->hdr.type = BINDER_TYPE_WEAK_BINDER;
		fp->binder = node->ptr;
		fp->cookie = node->cookie;
		if (node->proc)
			binder_inner_proc_lock(node->proc);
		binder_inc_node_nilocked(node,
					 fp->hdr.type == BINDER_TYPE_BINDER,
					 0, NULL);
		if (node->proc)
			binder_inner_proc_unlock(node->proc);
		trace_binder_transaction_ref_to_node(t, node, &src_rdata);
		binder_debug(BINDER_DEBUG_TRANSACTION,
			     "        ref %d desc %d -> node %d u%016llx\n",
			     src_rdata.debug_id, src_rdata.desc, node->debug_id,
			     (u64)node->ptr);
		binder_node_unlock(node);
	} else {
		struct binder_ref_data dest_rdata;

		binder_node_unlock(node);
		ret = binder_inc_ref_for_node(target_proc, node,
				fp->hdr.type == BINDER_TYPE_HANDLE,
				NULL, &dest_rdata);
		if (ret)
			goto done;

		fp->binder = 0;
		fp->handle = dest_rdata.desc;
		fp->cookie = 0;
		trace_binder_transaction_ref_to_ref(t, node, &src_rdata,
						    &dest_rdata);
		binder_debug(BINDER_DEBUG_TRANSACTION,
			     "        ref %d desc %d -> ref %d desc %d (node %d)\n",
			     src_rdata.debug_id, src_rdata.desc,
			     dest_rdata.debug_id, dest_rdata.desc,
			     node->debug_id);
	}
done:
	binder_put_node(node);
	return ret;
}