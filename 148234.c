ipsecmod_new(struct module_qstate* qstate, int id)
{
	struct ipsecmod_qstate* iq = (struct ipsecmod_qstate*)regional_alloc(
		qstate->region, sizeof(struct ipsecmod_qstate));
	memset(iq, 0, sizeof(*iq));
	qstate->minfo[id] = iq;
	if(!iq)
		return 0;
	/* Initialise it. */
	iq->enabled = qstate->env->cfg->ipsecmod_enabled;
	iq->is_whitelisted = ipsecmod_domain_is_whitelisted(
		(struct ipsecmod_env*)qstate->env->modinfo[id], qstate->qinfo.qname,
		qstate->qinfo.qname_len, qstate->qinfo.qclass);
	return 1;
}