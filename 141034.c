static int selinux_task_setpgid(struct task_struct *p, pid_t pgid)
{
	return current_has_perm(p, PROCESS__SETPGID);
}