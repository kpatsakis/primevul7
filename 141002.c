static void selinux_cred_transfer(struct cred *new, const struct cred *old)
{
	const struct task_security_struct *old_tsec = old->security;
	struct task_security_struct *tsec = new->security;

	*tsec = *old_tsec;
}