reply_find_answer_rrset(struct query_info* qinfo, struct reply_info* rep)
{
	uint8_t* sname = qinfo->qname;
	size_t snamelen = qinfo->qname_len;
	size_t i;
	for(i=0; i<rep->an_numrrsets; i++) {
		struct ub_packed_rrset_key* s = rep->rrsets[i];
		/* first match type, for query of qtype cname */
		if(ntohs(s->rk.type) == qinfo->qtype && 
			ntohs(s->rk.rrset_class) == qinfo->qclass && 
			snamelen == s->rk.dname_len &&
			query_dname_compare(sname, s->rk.dname) == 0) {
			return s;
		}
		/* follow CNAME chain (if any) */
		if(ntohs(s->rk.type) == LDNS_RR_TYPE_CNAME && 
			ntohs(s->rk.rrset_class) == qinfo->qclass && 
			snamelen == s->rk.dname_len &&
			query_dname_compare(sname, s->rk.dname) == 0) {
			get_cname_target(s, &sname, &snamelen);
		}
	}
	return NULL;
}