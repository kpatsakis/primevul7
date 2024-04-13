static void __giveup_altivec(struct task_struct *tsk)
{
	unsigned long msr;

	save_altivec(tsk);
	msr = tsk->thread.regs->msr;
	msr &= ~MSR_VEC;
#ifdef CONFIG_VSX
	if (cpu_has_feature(CPU_FTR_VSX))
		msr &= ~MSR_VSX;
#endif
	tsk->thread.regs->msr = msr;
}