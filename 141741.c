static inline struct audit_buffer *xfrm_audit_start(const char *op)
{
	struct audit_buffer *audit_buf = NULL;

	if (audit_enabled == AUDIT_OFF)
		return NULL;
	audit_buf = audit_log_start(audit_context(), GFP_ATOMIC,
				    AUDIT_MAC_IPSEC_EVENT);
	if (audit_buf == NULL)
		return NULL;
	audit_log_format(audit_buf, "op=%s", op);
	return audit_buf;
}