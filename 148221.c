ipsecmod_handle_response(struct module_qstate* qstate,
	struct ipsecmod_qstate* ATTR_UNUSED(iq),
	struct ipsecmod_env* ATTR_UNUSED(ie), int id)
{
	/* Pass to previous module if we are not enabled and whitelisted. */
	if(!(iq->enabled && iq->is_whitelisted)) {
		qstate->ext_state[id] = module_finished;
		return;
	}
	/* check if the response is for an A/AAAA query. */
	if((qstate->qinfo.qtype == LDNS_RR_TYPE_A ||
		qstate->qinfo.qtype == LDNS_RR_TYPE_AAAA) &&
		/* check that we had an answer for the A/AAAA query. */
		qstate->return_msg &&
		reply_find_answer_rrset(&qstate->return_msg->qinfo,
		qstate->return_msg->rep) &&
		/* check that another module didn't SERVFAIL. */
		qstate->return_rcode == LDNS_RCODE_NOERROR) {
		char type[16];
		sldns_wire2str_type_buf(qstate->qinfo.qtype, type,
			sizeof(type));
		verbose(VERB_ALGO, "ipsecmod: response for %s; generating IPSECKEY "
			"subquery", type);
		/* generate an IPSECKEY query. */
		if(!generate_request(qstate, id, qstate->qinfo.qname,
			qstate->qinfo.qname_len, LDNS_RR_TYPE_IPSECKEY,
			qstate->qinfo.qclass, 0)) {
			log_err("ipsecmod: could not generate subquery.");
			ipsecmod_error(qstate, id);
		}
		return;
	}
	/* we are done with the query. */
	qstate->ext_state[id] = module_finished;
}