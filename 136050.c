static inline void __switch_to_tm(struct task_struct *prev,
		struct task_struct *new)
{
	if (cpu_has_feature(CPU_FTR_TM)) {
		if (tm_enabled(prev) || tm_enabled(new))
			tm_enable();

		if (tm_enabled(prev)) {
			prev->thread.load_tm++;
			tm_reclaim_task(prev);
			if (!MSR_TM_ACTIVE(prev->thread.regs->msr) && prev->thread.load_tm == 0)
				prev->thread.regs->msr &= ~MSR_TM;
		}

		tm_recheckpoint_new_task(new);
	}
}