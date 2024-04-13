ipsecmod_clear(struct module_qstate* qstate, int id)
{
	if(!qstate)
		return;
	qstate->minfo[id] = NULL;
}