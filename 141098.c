static int selinux_task_wait(struct task_struct *p)
{
	return task_has_perm(p, current, PROCESS__SIGCHLD);
}