static int cred_has_perm(const struct cred *actor,
			 const struct cred *target,
			 u32 perms)
{
	u32 asid = cred_sid(actor), tsid = cred_sid(target);

	return avc_has_perm(asid, tsid, SECCLASS_PROCESS, perms, NULL);
}