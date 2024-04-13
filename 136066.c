static inline bool tm_enabled(struct task_struct *tsk)
{
	return tsk && tsk->thread.regs && (tsk->thread.regs->msr & MSR_TM);
}