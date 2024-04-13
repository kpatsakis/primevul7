static int selinux_task_kill(struct task_struct *p, struct siginfo *info,
				int sig, u32 secid)
{
	u32 perm;
	int rc;

	if (!sig)
		perm = PROCESS__SIGNULL; /* null signal; existence test */
	else
		perm = signal_to_av(sig);
	if (secid)
		rc = avc_has_perm(secid, task_sid(p),
				  SECCLASS_PROCESS, perm, NULL);
	else
		rc = current_has_perm(p, perm);
	return rc;
}