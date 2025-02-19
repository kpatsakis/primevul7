struct ub_packed_rrset_key* reply_find_rrset_section_ns(struct reply_info* rep,
	uint8_t* name, size_t namelen, uint16_t type, uint16_t dclass)
{
	size_t i;
	for(i=rep->an_numrrsets; i<rep->an_numrrsets+rep->ns_numrrsets; i++) {
		struct ub_packed_rrset_key* s = rep->rrsets[i];
		if(ntohs(s->rk.type) == type && 
			ntohs(s->rk.rrset_class) == dclass && 
			namelen == s->rk.dname_len &&
			query_dname_compare(name, s->rk.dname) == 0) {
			return s;
		}
	}
	return NULL;
}