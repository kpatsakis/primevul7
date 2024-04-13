static int selinux_task_getpgid(struct task_struct *p)
{
	return current_has_perm(p, PROCESS__GETPGID);
}