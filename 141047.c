static int ipc_alloc_security(struct task_struct *task,
			      struct kern_ipc_perm *perm,
			      u16 sclass)
{
	struct ipc_security_struct *isec;
	u32 sid;

	isec = kzalloc(sizeof(struct ipc_security_struct), GFP_KERNEL);
	if (!isec)
		return -ENOMEM;

	sid = task_sid(task);
	isec->sclass = sclass;
	isec->sid = sid;
	perm->security = isec;

	return 0;
}