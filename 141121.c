static void selinux_task_getsecid(struct task_struct *p, u32 *secid)
{
	*secid = task_sid(p);
}