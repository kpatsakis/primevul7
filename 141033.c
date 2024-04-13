static int selinux_task_getscheduler(struct task_struct *p)
{
	return current_has_perm(p, PROCESS__GETSCHED);
}