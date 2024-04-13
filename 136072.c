void enable_kernel_spe(void)
{
	WARN_ON(preemptible());

	msr_check_and_set(MSR_SPE);

	if (current->thread.regs && (current->thread.regs->msr & MSR_SPE)) {
		check_if_tm_restore_required(current);
		__giveup_spe(current);
	}
}