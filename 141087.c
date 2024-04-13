static int task_has_system(struct task_struct *tsk,
			   u32 perms)
{
	u32 sid = task_sid(tsk);

	return avc_has_perm(sid, SECINITSID_KERNEL,
			    SECCLASS_SYSTEM, perms, NULL);
}