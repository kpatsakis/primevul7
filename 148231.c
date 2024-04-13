generate_request(struct module_qstate* qstate, int id, uint8_t* name,
	size_t namelen, uint16_t qtype, uint16_t qclass, uint16_t flags)
{
	struct module_qstate* newq;
	struct query_info ask;
	ask.qname = name;
	ask.qname_len = namelen;
	ask.qtype = qtype;
	ask.qclass = qclass;
	ask.local_alias = NULL;
	log_query_info(VERB_ALGO, "ipsecmod: generate request", &ask);
	fptr_ok(fptr_whitelist_modenv_attach_sub(qstate->env->attach_sub));
	if(!(*qstate->env->attach_sub)(qstate, &ask,
		(uint16_t)(BIT_RD|flags), 0, 0, &newq)){
		log_err("Could not generate request: out of memory");
		return 0;
	}
	qstate->ext_state[id] = module_wait_subquery;
	return 1;
}