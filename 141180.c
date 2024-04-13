static int current_has_perm(const struct task_struct *tsk,
			    u32 perms)
{
	u32 sid, tsid;

	sid = current_sid();
	tsid = task_sid(tsk);
	return avc_has_perm(sid, tsid, SECCLASS_PROCESS, perms, NULL);
}