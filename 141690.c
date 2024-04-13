static inline void xfrm_audit_helper_usrinfo(bool task_valid,
					     struct audit_buffer *audit_buf)
{
	const unsigned int auid = from_kuid(&init_user_ns, task_valid ?
					    audit_get_loginuid(current) :
					    INVALID_UID);
	const unsigned int ses = task_valid ? audit_get_sessionid(current) :
		AUDIT_SID_UNSET;

	audit_log_format(audit_buf, " auid=%u ses=%u", auid, ses);
	audit_log_task_context(audit_buf);
}