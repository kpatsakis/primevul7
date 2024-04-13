static int restore_vsx(struct task_struct *tsk)
{
	if (cpu_has_feature(CPU_FTR_VSX)) {
		tsk->thread.used_vsr = 1;
		return 1;
	}

	return 0;
}