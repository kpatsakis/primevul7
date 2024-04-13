static int binder_put_node_cmd(struct binder_proc *proc,
			       struct binder_thread *thread,
			       void __user **ptrp,
			       binder_uintptr_t node_ptr,
			       binder_uintptr_t node_cookie,
			       int node_debug_id,
			       uint32_t cmd, const char *cmd_name)
{
	void __user *ptr = *ptrp;

	if (put_user(cmd, (uint32_t __user *)ptr))
		return -EFAULT;
	ptr += sizeof(uint32_t);

	if (put_user(node_ptr, (binder_uintptr_t __user *)ptr))
		return -EFAULT;
	ptr += sizeof(binder_uintptr_t);

	if (put_user(node_cookie, (binder_uintptr_t __user *)ptr))
		return -EFAULT;
	ptr += sizeof(binder_uintptr_t);

	binder_stat_br(proc, thread, cmd);
	binder_debug(BINDER_DEBUG_USER_REFS, "%d:%d %s %d u%016llx c%016llx\n",
		     proc->pid, thread->pid, cmd_name, node_debug_id,
		     (u64)node_ptr, (u64)node_cookie);

	*ptrp = ptr;
	return 0;
}