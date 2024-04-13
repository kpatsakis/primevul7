ipsecmod_operate(struct module_qstate* qstate, enum module_ev event, int id,
	struct outbound_entry* outbound)
{
	struct ipsecmod_env* ie = (struct ipsecmod_env*)qstate->env->modinfo[id];
	struct ipsecmod_qstate* iq = (struct ipsecmod_qstate*)qstate->minfo[id];
	verbose(VERB_QUERY, "ipsecmod[module %d] operate: extstate:%s event:%s",
		id, strextstate(qstate->ext_state[id]), strmodulevent(event));
	if(iq) log_query_info(VERB_QUERY, "ipsecmod operate: query",
		&qstate->qinfo);

	/* create ipsecmod_qstate. */
	if((event == module_event_new || event == module_event_pass) &&
		iq == NULL) {
		if(!ipsecmod_new(qstate, id)) {
			ipsecmod_error(qstate, id);
			return;
		}
		iq = (struct ipsecmod_qstate*)qstate->minfo[id];
	}
	if(iq && (event == module_event_pass || event == module_event_new)) {
		ipsecmod_handle_query(qstate, iq, ie, id);
		return;
	}
	if(iq && (event == module_event_moddone)) {
		ipsecmod_handle_response(qstate, iq, ie, id);
		return;
	}
	if(iq && outbound) {
		/* cachedb does not need to process responses at this time
		 * ignore it.
		cachedb_process_response(qstate, iq, ie, id, outbound, event);
		*/
		return;
	}
	if(event == module_event_error) {
		verbose(VERB_ALGO, "got called with event error, giving up");
		ipsecmod_error(qstate, id);
		return;
	}
	if(!iq && (event == module_event_moddone)) {
		/* during priming, module done but we never started. */
		qstate->ext_state[id] = module_finished;
		return;
	}

	log_err("ipsecmod: bad event %s", strmodulevent(event));
	ipsecmod_error(qstate, id);
	return;
}