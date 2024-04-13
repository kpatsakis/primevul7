static int selinux_getprocattr(struct task_struct *p,
			       char *name, char **value)
{
	const struct task_security_struct *__tsec;
	u32 sid;
	int error;
	unsigned len;

	if (current != p) {
		error = current_has_perm(p, PROCESS__GETATTR);
		if (error)
			return error;
	}

	rcu_read_lock();
	__tsec = __task_cred(p)->security;

	if (!strcmp(name, "current"))
		sid = __tsec->sid;
	else if (!strcmp(name, "prev"))
		sid = __tsec->osid;
	else if (!strcmp(name, "exec"))
		sid = __tsec->exec_sid;
	else if (!strcmp(name, "fscreate"))
		sid = __tsec->create_sid;
	else if (!strcmp(name, "keycreate"))
		sid = __tsec->keycreate_sid;
	else if (!strcmp(name, "sockcreate"))
		sid = __tsec->sockcreate_sid;
	else
		goto invalid;
	rcu_read_unlock();

	if (!sid)
		return 0;

	error = security_sid_to_context(sid, value, &len);
	if (error)
		return error;
	return len;

invalid:
	rcu_read_unlock();
	return -EINVAL;
}