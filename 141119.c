static int may_create_key(u32 ksid,
			  struct task_struct *ctx)
{
	u32 sid = task_sid(ctx);

	return avc_has_perm(sid, ksid, SECCLASS_KEY, KEY__CREATE, NULL);
}