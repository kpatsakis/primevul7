static void binder_proc_dec_tmpref(struct binder_proc *proc)
{
	binder_inner_proc_lock(proc);
	proc->tmp_ref--;
	if (proc->is_dead && RB_EMPTY_ROOT(&proc->threads) &&
			!proc->tmp_ref) {
		binder_inner_proc_unlock(proc);
		binder_free_proc(proc);
		return;
	}
	binder_inner_proc_unlock(proc);
}