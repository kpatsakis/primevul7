ipsecmod_inform_super(struct module_qstate* qstate, int id,
	struct module_qstate* super)
{
	struct ipsecmod_qstate* siq;
	log_query_info(VERB_ALGO, "ipsecmod: inform_super, sub is",
		&qstate->qinfo);
	log_query_info(VERB_ALGO, "super is", &super->qinfo);
	siq = (struct ipsecmod_qstate*)super->minfo[id];
	if(!siq) {
		verbose(VERB_ALGO, "super has no ipsecmod state");
		return;
	}

	if(qstate->return_msg) {
		struct ub_packed_rrset_key* rrset_key = reply_find_answer_rrset(
			&qstate->return_msg->qinfo, qstate->return_msg->rep);
		if(rrset_key) {
			/* We have an answer. */
			/* Copy to super's region. */
			rrset_key = packed_rrset_copy_region(rrset_key, super->region, 0);
			siq->ipseckey_rrset = rrset_key;
			if(!rrset_key) {
				log_err("ipsecmod: out of memory.");
			}
		}
	}
	/* Notify super to proceed. */
	siq->ipseckey_done = 1;
}