static int selinux_task_getsid(struct task_struct *p)
{
	return current_has_perm(p, PROCESS__GETSESSION);
}