int arch_dup_task_struct(struct task_struct *dst, struct task_struct *src)
{
	flush_all_to_thread(src);
	/*
	 * Flush TM state out so we can copy it.  __switch_to_tm() does this
	 * flush but it removes the checkpointed state from the current CPU and
	 * transitions the CPU out of TM mode.  Hence we need to call
	 * tm_recheckpoint_new_task() (on the same task) to restore the
	 * checkpointed state back and the TM mode.
	 *
	 * Can't pass dst because it isn't ready. Doesn't matter, passing
	 * dst is only important for __switch_to()
	 */
	__switch_to_tm(src, src);

	*dst = *src;

	clear_task_ebb(dst);

	return 0;
}